#include <algorithm>
#include <cstdlib>
#include <exception>
#include <expected>
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

std::string build_last_errors_openssl()
{
    std::shared_ptr<BIO> stream(BIO_new(BIO_s_mem()), BIO_free);
    ERR_print_errors(stream.get());
    char*  buf = nullptr;
    size_t len = BIO_get_mem_data(stream.get(), &buf);

    std::string msg(buf, len);
    return msg;
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
    for (auto& byte : salt.bytes)
    {
        // NOLINTNEXTLINE
        char hex[3] = { 0 }; // Buffer to hold 2 hex chars + null terminator
        is.read(hex, 2);     // Read 2 hex characters
        if (!is || is.gcount() != 2)
        {
            throw std::runtime_error("Failed to read 2 hex characters");
        }
        byte = static_cast<unsigned char>(std::stoi(hex, nullptr, 16));
    }
    return is;
}

std::expected<salt_t, std::string> gen_salt() noexcept
{
    salt_t salt{};
    if (!RAND_bytes(salt.bytes.data(), salt.bytes.size()))
    {
        return std::unexpected(build_last_errors_openssl());
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
             const std::string&           password,
             const salt_t&                salt,
             const std::filesystem::path& out_file)
{
    constexpr int key_len    = 16;    // AES-128
    constexpr int iv_len     = 16;    // AES-CTR IV length
    constexpr int iterations = 10000; // see: PBKDF2_ITER_DEFAULT 10000

    // generate key and iv like in openssl(3.2.2) see: apps/enc.c:560
    std::array<unsigned char, key_len + iv_len> key_and_iv;

    // generate key and IV using PBKDF2
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt.bytes.data(),
                           static_cast<int>(salt.bytes.size()),
                           iterations,
                           EVP_sha256(),
                           key_len + iv_len,
                           key_and_iv.data()))
    {
        build_last_errors_openssl();
    }

    unsigned char key[key_len];
    unsigned char iv[iv_len];
    // split key and iv
    std::memcpy(key, key_and_iv.data(), key_len);
    std::memcpy(iv, key_and_iv.data() + key_len, iv_len);

    std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(),
                                        EVP_CIPHER_CTX_free);
    if (!ctx)
    {
        build_last_errors_openssl();
    }

    if (!EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_ctr(), nullptr, key, iv))
    {
        build_last_errors_openssl();
    }

    std::ifstream ifs(in_file, std::ios::binary);

    if (!ifs)
    {
        std::string msg("Error opening input file: ");
        msg += in_file.string();
        throw std::runtime_error(msg);
    }

    std::ofstream ofs(out_file, std::ios::binary);

    constexpr int buf_size = 4096;
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
            build_last_errors_openssl();
        }

        ofs.write(reinterpret_cast<char*>(out_buf.data()), out_len);
        ifs.read(reinterpret_cast<char*>(buf_in),
                 static_cast<std::streamsize>(buf_size));
    }

    int final_len = 0;
    if (!EVP_EncryptFinal_ex(ctx.get(), out_buf.data(), &final_len))
    {
        build_last_errors_openssl();
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

    std::ifstream ifs(in_file, std::ios::binary);
    if (!ifs)
    {
        std::string msg("error: opening input file: ");
        msg += in_file.string();
        throw std::runtime_error(msg);
    }

    unsigned char key_and_iv[key_len + iv_len];
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt.bytes.data(),
                           static_cast<int>(salt.bytes.size()),
                           iterations,
                           EVP_sha256(),
                           key_len + iv_len,
                           key_and_iv))
    {
        build_last_errors_openssl();
    }

    unsigned char key[key_len];
    unsigned char iv[iv_len];

    std::memcpy(key, key_and_iv, key_len);
    std::memcpy(iv, key_and_iv + key_len, iv_len);

    std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(),
                                        EVP_CIPHER_CTX_free);
    if (!ctx)
    {
        build_last_errors_openssl();
    }

    if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_ctr(), nullptr, key, iv))
    {
        build_last_errors_openssl();
    }

    std::ofstream ofs(out_file, std::ios::binary);
    if (!ofs)
    {
        std::string msg("error: opening output file: ");
        msg += out_file.string();
        throw std::runtime_error(msg);
    }

    constexpr int buf_size = 4096;
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
            build_last_errors_openssl();
        }
        ofs.write(reinterpret_cast<const char*>(buf_out.data()), out_len);
        ifs.read(reinterpret_cast<char*>(buf_in),
                 static_cast<std::streamsize>(buf_size));
    }

    int final_len = 0;
    if (1 != EVP_DecryptFinal_ex(ctx.get(), buf_out.data(), &final_len))
    {
        build_last_errors_openssl();
    }
    ofs.write(reinterpret_cast<const char*>(buf_out.data()), final_len);
}

void validate_command(const std::string& command)
{
    static const std::vector<std::string> valid_commands = { "enc",
                                                             "dec",
                                                             "gen_salt" };

    namespace po = boost::program_options;

    if (std::ranges::find(valid_commands, command) == valid_commands.end())
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
        po::options_description help("help");
        help.add_options()("help,v", "print this help");

        po::options_description encoding_options("encoding");
        // clang-format off
        encoding_options.add_options()
        ("cmd", po::value<std::string>(&arg_cmd)->required()->notifier(om::validate_command), "[enc, dec, gen_salt]")
        ("pass", po::value<std::string>(&arg_pass), "your password like in openssl -pass option")
        ("salt", po::value<std::string>(&arg_salt), "your salt in hex format 16 bytes 32 chars")
        ("in_file,i", po::value<std::string>(&arg_in), "path to input file")
        ("out_file,o", po::value<std::string>(&arg_out), "path to output file")
        ;
        // clang-format on
        po::positional_options_description pd;
        pd.add("cmd", 1);

        po::options_description all("modes");
        all.add(help).add(encoding_options);

        po::command_line_parser parser{ argc, argv };
        parser.options(all).positional(pd);
        po::parsed_options parsed_options = parser.run();

        po::variables_map vm;
        po::store(parsed_options, vm);

        if (vm.count("help"))
        {
            std::cout << all << std::endl;
            return EXIT_SUCCESS;
        }

        po::notify(vm); // skip if "help" any errors
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    using namespace om;

    if (arg_cmd == "gen_salt")
    {
        auto salt = gen_salt();
        if (salt.has_value())
        {
            std::cout << salt.value() << std::endl;
        }
        else
        {
            std::cout << salt.error() << std::endl;
        }
        return std::cout.fail();
    }

    if (arg_cmd == "enc")
    {
        try
        {
            salt_t salt{};
            std::stringstream(arg_salt) >> salt;
            encrypt(arg_in, arg_pass, salt, arg_out);
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
            salt_t salt{};
            std::stringstream(arg_salt) >> salt;

            decrypt(arg_in, arg_out, arg_pass, salt);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "error: decrypt failed: " << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
