#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(int argc, const char * const argv[])
{
  //aes_init();
  string key { gen_key_16() };

  //text to encrypt
  string plaintext;
  if (argc <= 1) {
    plaintext= "crypto project 2";
  } else {
    plaintext = string(argv[1]);
    //plaintext.resize(plaintext.find('\0'), '\0');
  }
  cout << "plaintext  : " << plaintext << endl;

  //encrypt
  string encoded = aes_128_gcm_encrypt(plaintext, key);
  cout << "ciphertext : " << encoded << endl;

  //decrypt
  string out = aes_128_gcm_decrypt(encoded, key);
  cout << "Dec(Enc(m)): " << out << endl;
}
