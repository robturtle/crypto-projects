#include "crypto.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <string>
#include <fstream>
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
    OpenSSL_add_all_ciphers();
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


  /* class Rsa */
  Rsa::Rsa(const size_t keyLength, const size_t pubExponent) {
    initNew(keyLength, pubExponent);
  }

  Rsa::Rsa(const string& pubFname, const string& priFname) {
    bool existsPubFile = ifstream{ pubFname }.good();
    bool existsPriFile = ifstream{ priFname }.good();
    if (existsPubFile || existsPriFile) {
      if (existsPubFile) {
        ifstream in { pubFname };
        string pubKeyString { istreambuf_iterator<char>(in), istreambuf_iterator<char>() };
        readPublicKey(pubKeyString);
        hasPub = true;
      }
      if (existsPriFile) {
        ifstream in { priFname };
        string priKeyString { istreambuf_iterator<char>(in), istreambuf_iterator<char>() };
        readPrivateKey(priKeyString);
        hasPri = true;
      }
      maxEncSize = RSA_size(keypair) - 41 - 1 - 1;
    } else {
      initNew();
      ofstream pubOut { pubFname };
      ofstream priOut { priFname };
      write(pubOut, priOut);
    }
  }

  void Rsa::initNew(size_t keyLength, size_t pubExponent) {
    keypair = RSA_generate_key(keyLength, pubExponent, nullptr, nullptr);
    maxEncSize = keyLength/8 - 41/* TODO explain */ - 1/* '\0' */ - 1 /* TODO explain */;
    hasPub = true;
    hasPri = true;
  }


  void Rsa::write(ostream& pubOut, ostream& priOut) {
    if (hasPub) {
      BIO *pub = BIO_new(BIO_s_mem());
      PEM_write_bio_RSAPublicKey(pub, keypair);
      size_t pub_len = BIO_pending(pub);
      char *pub_key = (char*)malloc(pub_len + 1);
      BIO_read(pub, pub_key, pub_len);
      pub_key[pub_len] = '\0';
      pubOut << pub_key << endl;
      BIO_free_all(pub);
      free(pub_key);
    }
    if (hasPri) {
      BIO *pri = BIO_new(BIO_s_mem());
      PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
      size_t pri_len = BIO_pending(pri);
      char *pri_key = (char*)malloc(pri_len + 1);
      BIO_read(pri, pri_key, pri_len);
      pri_key[pri_len] = '\0';
      priOut << pri_key << endl;
      BIO_free_all(pri);
      free(pri_key);
    }
  }

  void Rsa::readPublicKey(const string& key) {
    BIO *bo = BIO_new(BIO_s_mem());
    BIO_write(bo, &key[0], key.length()+1);
    PEM_read_bio_RSAPublicKey(bo, &keypair, nullptr, nullptr);
  }

  void Rsa::readPrivateKey(const string& key) {
    BIO *bo = BIO_new(BIO_s_mem());
    BIO_write(bo, &key[0], key.length()+1);
    PEM_read_bio_RSAPrivateKey(bo, &keypair, nullptr, nullptr);
  }


  RSA* Rsa::getKeypair() const {
    return keypair;
  }

  string Rsa::readPlaintext(istream &in) {
    string plaintext;
    readPlaintext(in, plaintext);
    return plaintext;
  }

  void Rsa::readPlaintext(istream &in, string &plaintext) {
    plaintext.resize(maxEncSize);
    in.read(&plaintext[0], maxEncSize);
    plaintext.resize(in.gcount());
  }

  string Rsa::encrypt(const string& plaintext) {
    string ciphertext;
    ciphertext.resize(RSA_size(keypair));
    int encLen = RSA_public_encrypt(plaintext.length()+1,
                                    (word*)plaintext.c_str(),
                                    (word*)&ciphertext[0],
                                    keypair, RSA_PKCS1_OAEP_PADDING);
    handleError(encLen);
    return base64::encode(ciphertext);
  }

  string Rsa::decrypt(const string& base64_ciphertext) {
    string ciphertext { base64::decode(base64_ciphertext) };
    string plaintext;
    plaintext.resize(RSA_size(keypair));
    int decLen = RSA_private_decrypt(RSA_size(keypair),
                                     (word*)ciphertext.c_str(),
                                     (word*)&plaintext[0],
                                     keypair, RSA_PKCS1_OAEP_PADDING);
    handleError(decLen);
    return plaintext;
  }

  void Rsa::handleError(int retVal) {
    if (retVal >= 0) { return; }
    string err;
    err.resize(130); // TODO explain
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), &err[0]);
    throw err;
  }

  /* End of class Rsa */


} /* ns crypto */
