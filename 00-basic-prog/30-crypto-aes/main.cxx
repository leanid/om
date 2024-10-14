#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

void throw_errors()
{
    std::shared_ptr<BIO> stream(BIO_new(BIO_s_mem()), BIO_free);
    ERR_print_errors(stream.get());
    char*  buf = nullptr;
    size_t len = BIO_get_mem_data(stream.get(), &buf);

    std::string msg(buf, len);
    throw std::runtime_error(msg);
}

/// This is direct implementation of:
/// @code
/// openssl enc -aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml -out
/// ru.yaml.enc
/// @endcode
void encrypt(const std::string& in_file,
             const std::string& out_file,
             const std::string& password)
{
    const int  key_len    = 16;         // AES-128
    const int  iv_len     = 16;         // AES-CTR IV length
    const int  iterations = 10000;      // see: PBKDF2_ITER_DEFAULT 10000
    const int  salt_len   = 8;          // default like in openssl
    const char magic[]    = "Salted__"; // default like in openssl

    unsigned char salt[salt_len];
    if (!RAND_bytes(salt, salt_len))
    {
        throw_errors();
    }

    // generate key and iv like in openssl(3.2.2) see: apps/enc.c:560
    unsigned char key_and_iv[key_len + iv_len];

    // generate key and IV using PBKDF2
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt,
                           salt_len,
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
        msg += in_file;
        throw std::runtime_error(msg);
    }

    std::ofstream ofs(out_file, std::ios::binary);

    ofs.write(magic, sizeof(magic) - 1); // without end \0
    // std::cout << "write magic: " << magic << std::endl;
    ofs.write(reinterpret_cast<char*>(salt), salt_len);
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

/// This is direct implementation of:
/// @code
/// openssl enc -d -aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml.enc -out
/// ru.yaml.enc.dec
/// @endcode
void decrypt(const std::string& in_file,
             const std::string& out_file,
             const std::string& password)
{
    const int  key_len    = 16;         // AES-128
    const int  iv_len     = 16;         // AES-CTR IV length
    const int  iterations = 10000;      // see: PBKDF2_ITER_DEFAULT 10000
    const int  salt_len   = 8;          // default like in openssl
    const char magic[]    = "Salted__"; // default like in openssl

    std::ifstream ifs(in_file, std::ios::binary);
    if (!ifs)
    {
        std::string msg("error: opening input file: ");
        msg += in_file;
        throw std::runtime_error(msg);
    }

    char file_magic[sizeof(magic) - 1]{};
    ifs.read(file_magic, sizeof(file_magic));
    if (std::memcmp(file_magic, magic, sizeof(magic) - 1) != 0)
    {
        throw std::runtime_error("error: bad magic constant");
    }

    unsigned char salt[salt_len];
    ifs.read(reinterpret_cast<char*>(salt), salt_len);

    unsigned char key_and_iv[key_len + iv_len];
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           static_cast<int>(password.length()),
                           salt,
                           salt_len,
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
        msg += out_file;
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

int main()
{
    const std::string password     = "leanid";
    const std::string in_file      = "ru.yaml";
    const std::string out_file_enc = "ru.yaml.enc";
    const std::string out_file_dec = "ru.yaml.enc.dec";

    try
    {
        encrypt(in_file, out_file_enc, password);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: encrypt failed: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        decrypt(out_file_enc, out_file_dec, password);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: decrypt failed: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
