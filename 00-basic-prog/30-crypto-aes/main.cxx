#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
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

    // Generate key and IV using PBKDF2
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

    if (1 != EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_ctr(), nullptr, key, iv))
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
    std::cout << "write magic: " << magic << std::endl;
    ofs.write(reinterpret_cast<char*>(salt), salt_len);
    std::cout << "write salt: ";
    for (unsigned i = 0; i < 8; i++)
    {
        std::cout << std::hex << std::setw(2) << unsigned(salt[i]);
    }
    std::cout << std::endl;

    std::vector<unsigned char> buffer(1024);
    std::vector<unsigned char> cipherBuffer(
        1024 + EVP_CIPHER_block_size(EVP_aes_128_ctr()));

    int outLen = 0;

    ifs.read(reinterpret_cast<char*>(buffer.data()),
             static_cast<std::streamsize>(buffer.size()));

    while (ifs.gcount())
    {
        if (1 != EVP_EncryptUpdate(ctx.get(),
                                   cipherBuffer.data(),
                                   &outLen,
                                   buffer.data(),
                                   static_cast<int>(ifs.gcount())))
        {
            throw_errors();
        }
        ofs.write(reinterpret_cast<const char*>(cipherBuffer.data()), outLen);
        ifs.read(reinterpret_cast<char*>(buffer.data()),
                 static_cast<std::streamsize>(buffer.size()));
    }

    if (1 != EVP_EncryptFinal_ex(ctx.get(), cipherBuffer.data(), &outLen))
    {
        throw_errors();
    }
    ofs.write(reinterpret_cast<const char*>(cipherBuffer.data()), outLen);
}

int main()
{
    const std::string password = "leanid";
    const std::string in_file  = "ru.yaml";
    const std::string out_file = "ru.yaml.enc";

    try
    {
        encrypt(in_file, out_file, password);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: encrypt failed: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
