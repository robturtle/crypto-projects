#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "base64.h"

using namespace std;
using word  = unsigned char;
using words = vector<word>;

void aes_init() {
  static int init = 0;
  if (init == 0) {
    EVP_CIPHER_CTX e_ctx, d_ctx;
    // initialize openssl ciphers
    OpenSSL_add_all_ciphers();
    // initialize random number generator (for IVs)
    int rv = RAND_load_file("/dev/urandom", 32);
  }
}

namespace crypto {

  string gen_key_16() {
    word key_bytes[16];
    RAND_bytes(key_bytes, sizeof(key_bytes));
    string key{ (char *)key_bytes, sizeof(key_bytes) };
    return base64::encode(key);
  }

  // return a base64 ciphertext
  string aes_128_gcm_encrypt(const string& plaintext, const string& base64_key) {
    aes_init();

    string key { base64::decode(base64_key) };
    size_t enc_length = plaintext.length() * 3;
    if (enc_length < 48) enc_length = 48;
    words output;
    output.resize(enc_length, '\0');

    word tag[AES_BLOCK_SIZE];
    word iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, sizeof(iv));
    copy(iv, iv+16, output.begin()+16);

    int actual_size = 0, final_size = 0;
    EVP_CIPHER_CTX* e_ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit(e_ctx, EVP_aes_128_gcm(), (const word*) key.c_str(), iv);
    EVP_EncryptUpdate(e_ctx, &output[32], &actual_size,
                      (const word*) plaintext.data(), plaintext.length());
    EVP_EncryptFinal(e_ctx, &output[32+actual_size], &final_size);
    EVP_CIPHER_CTX_ctrl(e_ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    copy(tag, tag+16, output.begin());
    copy(iv, iv+16, output.begin()+16);
    output.resize(32 + actual_size + final_size);
    EVP_CIPHER_CTX_free(e_ctx);
    return base64::encode(string(output.begin(), output.end()));
  }


  string aes_128_gcm_decrypt(const string& base64_ciphertext, const string& base64_key)
  {
    aes_init();

    string key     { base64::decode(base64_key) };
    string decoded { base64::decode(base64_ciphertext) };
    vector<word> ciphertext(decoded.begin(), decoded.end());
    word tag[AES_BLOCK_SIZE];
    word iv[AES_BLOCK_SIZE];
    copy( ciphertext.begin(), ciphertext.begin()+16, tag);
    copy( ciphertext.begin()+16, ciphertext.begin()+32, iv);
    vector<word> plaintext;
    plaintext.resize(ciphertext.size(), '\0');

    int actual_size=0, final_size=0;
    EVP_CIPHER_CTX *d_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(d_ctx, EVP_aes_128_gcm(), (const word*) key.c_str(), iv);
    EVP_DecryptUpdate(d_ctx, &plaintext[0], &actual_size, &ciphertext[32], ciphertext.size()-32 );
    EVP_CIPHER_CTX_ctrl(d_ctx, EVP_CTRL_GCM_SET_TAG, 16, tag);
    EVP_DecryptFinal(d_ctx, &plaintext[actual_size], &final_size);
    EVP_CIPHER_CTX_free(d_ctx);
    plaintext.resize(actual_size + final_size, '\0');

    return string(plaintext.begin(), plaintext.end());
  }

} /* ns crypto */
