#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(void) {
  string plaintext, key;
  getline(cin, plaintext);
  getline(cin, key);
  cout << aes_128_gcm_encrypt(plaintext, key) << endl;
}
