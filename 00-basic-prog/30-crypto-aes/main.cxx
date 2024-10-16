#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <boost/program_options.hpp>

namespace om
{

void throw_errors()
{
    std::shared_ptr<BIO> stream(BIO_new(BIO_s_mem()), BIO_free);
    ERR_print_errors(stream.get());
    char*  buf = nullptr;
    size_t len = BIO_get_mem_data(stream.get(), &buf);

    std::string msg(buf, len);
    throw std::runtime_error(msg);
}

struct salt_t
{
    std::array<unsigned char, 16> bytes;
    auto                          operator<=>(const salt_t&) const = default;
};

std::ostream& operator<<(std::ostream& os, const salt_t& salt)
{
    auto flags = os.flags();
    os << std::hex << std::setfill('0');
    for (auto byte : salt.bytes)
    {
        os << std::setw(2) << static_cast<unsigned>(byte);
    }
    os.flags(flags);
    return os;
}

std::istream& operator>>(std::istream& is, salt_t& salt)
{
    for (size_t i = 0; i < salt.bytes.size(); ++i)
    {
        char hex[3] = { 0 }; // Buffer to hold 2 hex chars + null terminator
        is.read(hex, 2);     // Read 2 hex characters
        if (is.gcount() != 2)
        {
            throw std::runtime_error("Failed to read 2 hex characters");
        }
        salt.bytes[i] = static_cast<unsigned char>(std::stoi(hex, nullptr, 16));
    }
    return is;
}

salt_t gen_salt()
{
    salt_t salt{};
    if (!RAND_bytes(salt.bytes.data(), salt.bytes.size()))
    {
        throw_errors();
    }
    return salt;
}

/// This is direct implementation of (example):
/// @code
/// openssl enc -salt -S "cc6d4b31bc04f5820f4e27bc3ddbff72" -saltlen 16
/// -aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml -out ru.yaml.enc.ctr
/// @endcode
/// salt maximum value is 16 bytes:
/// https://docs.openssl.org/3.3/man1/openssl-enc/#options
void encrypt(const std::filesystem::path& in_file,
             const std::filesystem::path& out_file,
             const std::string&           password,
             const salt_t&                salt)
{
    const int key_len    = 16;    // AES-128
    const int iv_len     = 16;    // AES-CTR IV length
    const int iterations = 10000; // see: PBKDF2_ITER_DEFAULT 10000
    // const char magic[]    = "Salted__"; // default like in openssl

    // generate key and iv like in openssl(3.2.2) see: apps/enc.c:560
    unsigned char key_and_iv[key_len + iv_len];

    // generate key and IV using PBKDF2
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt.bytes.data(),
                           salt.bytes.size(),
                           iterations,
                           EVP_sha256(),
                           key_len + iv_len,
                           key_and_iv))
    {
        throw_errors();
    }

    unsigned char key[key_len];
    unsigned char iv[iv_len];

    std::memcpy(key, key_and_iv, key_len);
    std::memcpy(iv, key_and_iv + key_len, iv_len);

    std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(),
                                        EVP_CIPHER_CTX_free);
    if (!ctx)
    {
        throw_errors();
    }

    if (!EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_ctr(), nullptr, key, iv))
    {
        throw_errors();
    }

    std::ifstream ifs(in_file, std::ios::binary);

    if (!ifs)
    {
        std::string msg("Error opening input file: ");
        msg += in_file.string();
        throw std::runtime_error(msg);
    }

    std::ofstream ofs(out_file, std::ios::binary);

    // ofs.write(magic, sizeof(magic) - 1); // without end \0
    // std::cout << "write magic: " << magic << std::endl;
    // ofs.write(reinterpret_cast<char*>(salt), salt_len);
    // std::cout << "write salt: ";
    // for (unsigned i = 0; i < 8; i++)
    // {
    //     std::cout << std::hex << std::setw(2) << unsigned(salt[i]);
    // }
    // std::cout << std::dec << std::endl;

    constexpr int buf_size = 1024;
    unsigned char buf_in[buf_size];
    // see:
    // https://docs.openssl.org/3.1/man3/EVP_EncryptInit/#description
    // EVP_DecryptUpdate() should have sufficient room for (inl +
    // cipher_block_size) bytes
    std::vector<unsigned char> out_buf(
        buf_size + EVP_CIPHER_block_size(EVP_aes_128_ctr()));

    ifs.read(reinterpret_cast<char*>(buf_in),
             static_cast<std::streamsize>(buf_size));

    while (ifs.gcount())
    {
        int out_len = 0;
        if (!EVP_EncryptUpdate(ctx.get(),
                               out_buf.data(),
                               &out_len,
                               buf_in,
                               static_cast<int>(ifs.gcount())))
        {
            throw_errors();
        }

        ofs.write(reinterpret_cast<char*>(out_buf.data()), out_len);
        ifs.read(reinterpret_cast<char*>(buf_in),
                 static_cast<std::streamsize>(buf_size));
    }

    int final_len = 0;
    if (!EVP_EncryptFinal_ex(ctx.get(), out_buf.data(), &final_len))
    {
        throw_errors();
    }
    ofs.write(reinterpret_cast<const char*>(out_buf.data()), final_len);
}

/// This is direct implementation of (example):
/// @code
/// openssl enc -d -salt -S "cc6d4b31bc04f5820f4e27bc3ddbff72" -saltlen 16
/// -aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml.enc.ctr -out
/// ru.yaml.enc.ctr.dec
/// @endcode
void decrypt(const std::filesystem::path& in_file,
             const std::filesystem::path& out_file,
             const std::string&           password,
             const salt_t&                salt)
{
    const int key_len    = 16;    // AES-128
    const int iv_len     = 16;    // AES-CTR IV length
    const int iterations = 10000; // see: PBKDF2_ITER_DEFAULT 10000
    // const int  salt_len   = 8;          // default like in openssl
    // const char magic[]    = "Salted__"; // default like in openssl

    std::ifstream ifs(in_file, std::ios::binary);
    if (!ifs)
    {
        std::string msg("error: opening input file: ");
        msg += in_file.string();
        throw std::runtime_error(msg);
    }

    // char file_magic[sizeof(magic) - 1]{};
    // ifs.read(file_magic, sizeof(file_magic));
    // if (std::memcmp(file_magic, magic, sizeof(magic) - 1) != 0)
    // {
    //     throw std::runtime_error("error: bad magic constant");
    // }

    // unsigned char salt[salt_len];
    // ifs.read(reinterpret_cast<char*>(salt), salt_len);

    unsigned char key_and_iv[key_len + iv_len];
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt.bytes.data(),
                           salt.bytes.size(),
                           iterations,
                           EVP_sha256(),
                           key_len + iv_len,
                           key_and_iv))
    {
        throw_errors();
    }

    unsigned char key[key_len];
    unsigned char iv[iv_len];

    std::memcpy(key, key_and_iv, key_len);
    std::memcpy(iv, key_and_iv + key_len, iv_len);

    std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(),
                                        EVP_CIPHER_CTX_free);
    if (!ctx)
    {
        throw_errors();
    }

    if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_ctr(), nullptr, key, iv))
    {
        throw_errors();
    }

    std::ofstream ofs(out_file, std::ios::binary);
    if (!ofs)
    {
        std::string msg("error: opening output file: ");
        msg += out_file.string();
        throw std::runtime_error(msg);
    }

    constexpr int buf_size = 1024;
    unsigned char buf_in[buf_size];
    // see:
    // https://docs.openssl.org/3.1/man3/EVP_EncryptInit/#description
    // EVP_DecryptUpdate() should have sufficient room for (inl +
    // cipher_block_size) bytes
    std::vector<unsigned char> buf_out(
        buf_size + EVP_CIPHER_block_size(EVP_aes_128_ctr()));

    ifs.read(reinterpret_cast<char*>(buf_in),
             static_cast<std::streamsize>(buf_size));

    while (ifs.gcount())
    {
        int out_len = 0;
        if (!EVP_DecryptUpdate(ctx.get(),
                               buf_out.data(),
                               &out_len,
                               buf_in,
                               static_cast<int>(ifs.gcount())))
        {
            throw_errors();
        }
        ofs.write(reinterpret_cast<const char*>(buf_out.data()), out_len);
        ifs.read(reinterpret_cast<char*>(buf_in),
                 static_cast<std::streamsize>(buf_size));
    }

    int final_len = 0;
    if (1 != EVP_DecryptFinal_ex(ctx.get(), buf_out.data(), &final_len))
    {
        throw_errors();
    }
    ofs.write(reinterpret_cast<const char*>(buf_out.data()), final_len);
}

void validate_command(const std::string& command)
{
    static const std::vector<std::string> valid_commands = { "enc",
                                                             "dec",
                                                             "gen_salt" };

    namespace po = boost::program_options;

    if (std::find(valid_commands.begin(), valid_commands.end(), command) ==
        valid_commands.end())
    {
        throw po::validation_error(
            po::validation_error::invalid_option_value, "command", command);
    }
}
} // namespace om

int main(int argc, char* argv[])
{
    std::string arg_cmd;
    std::string arg_pass;
    std::string arg_salt;
    std::string arg_in;
    std::string arg_out;

    try
    {
        namespace po = boost::program_options;
        po::options_description help("how to");
        help.add_options()("help,v", "print this help");

        po::options_description encoding_options("encoding");
        // clang-format off
        encoding_options.add_options()
        ("enc", po::value<std::string>(&arg_cmd)->required()->notifier(om::validate_command),
                   "one of [enc, dec, gen_salt]")
        ("pass", po::value<std::string>(&arg_pass), "your password like in openssl -pass option")
        ("salt", po::value<std::string>(&arg_salt), "your salt in hex format 16 bytes 32 chars")
        ("in_file,i", po::value<std::string>(&arg_in), "path to input file")
        ("out_file,o", po::value<std::string>(&arg_out), "path to output file")
        ;
        // clang-format on
        po::positional_options_description pd;
        pd.add("enc", 1);
        po::options_description decoding_options("decoding");
        // clang-format off
        decoding_options.add_options()
        ("dec", po::value<std::string>(&arg_cmd)->required()->notifier(om::validate_command),
                   "one of [enc, dec, gen_salt]")
        ("pass", po::value<std::string>(&arg_pass), "your password like in openssl -pass option")
        ("salt", po::value<std::string>(&arg_salt), "your salt in hex format 16 bytes 32 chars")
        ("in_file,i", po::value<std::string>(&arg_in), "path to input file")
        ("out_file,o", po::value<std::string>(&arg_out), "path to output file")
        ;
        // clang-format on
        po::positional_options_description pd2;
        pd.add("dec", 1);

        po::command_line_parser parser{ argc, argv };
        parser.options(encoding_options)
            .positional(pd)
            .options(decoding_options)
            .positional(pd2)
            .options(help);
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        po::store(parsed_options, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << encoding_options << std::endl;
            return EXIT_SUCCESS;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    // const std::string password     = "leanid";
    // const std::string in_file      = "ru.yaml";
    // const std::string out_file_enc = "ru.yaml.enc";
    // const std::string out_file_dec = "ru.yaml.enc.dec";

    using namespace om;

    if (arg_cmd == "gen_salt")
    {
        salt_t salt = gen_salt();
        std::cout << salt << std::endl;
        return std::cout.fail();
    }

    if (arg_cmd == "enc")
    {
        try
        {
            std::stringstream ss(arg_salt);
            salt_t            salt{};
            ss >> salt;
            encrypt(arg_in, arg_out, arg_pass, salt);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "error: encrypt failed: " << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (arg_cmd == "dec")
    {
        try
        {
            std::stringstream ss(arg_salt);
            salt_t            salt{};
            ss >> salt;

            decrypt(arg_in, arg_out, arg_pass, salt);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "error: decrypt failed: " << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
