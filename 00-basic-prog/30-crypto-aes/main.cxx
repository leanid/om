#include <fstream>
#include <cstring>
#include <iostream>
#include <vector>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

void encrypt(const std::string& inFile,
             const std::string& outFile,
             const std::string& password)
{
    const int keyLength  = 16; // AES-128
    const int ivLength   = 16; // AES-CTR IV length
    const int iterations = 10000;

    unsigned char key[keyLength];
    unsigned char iv[ivLength];

    // Generate key and IV using PBKDF2
    if (!PKCS5_PBKDF2_HMAC(password.c_str(),
                           password.length(),
                           nullptr,
                           0,
                           iterations,
                           EVP_sha256(),
                           keyLength,
                           key))
    {
        handleErrors();
    }

    if (!RAND_bytes(iv, ivLength))
    {
        handleErrors();
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        handleErrors();
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, key, iv))
    {
        handleErrors();
    }

    std::ifstream ifs(inFile, std::ios::binary);
    std::ofstream ofs(outFile, std::ios::binary);

    ofs.write(reinterpret_cast<const char*>(iv), ivLength);

    std::vector<unsigned char> buffer(1024);
    std::vector<unsigned char> cipherBuffer(
        1024 + EVP_CIPHER_block_size(EVP_aes_128_ctr()));
    int outLen;

    while (ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size()))
    {
        if (1 !=
            EVP_EncryptUpdate(
                ctx, cipherBuffer.data(), &outLen, buffer.data(), ifs.gcount()))
        {
            handleErrors();
        }
        ofs.write(reinterpret_cast<const char*>(cipherBuffer.data()), outLen);
    }

    if (1 != EVP_EncryptFinal_ex(ctx, cipherBuffer.data(), &outLen))
    {
        handleErrors();
    }
    ofs.write(reinterpret_cast<const char*>(cipherBuffer.data()), outLen);

    EVP_CIPHER_CTX_free(ctx);
}

int main()
{
    const std::string password = "leanid";
    const std::string inFile   = "ru.yaml";
    const std::string outFile  = "ru.yaml.enc";

    encrypt(inFile, outFile, password);

    std::cout << "File encrypted successfully." << std::endl;
    return 0;
}
