#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(void) {
  string ciphertext, key;
  getline(cin, ciphertext);
  getline(cin, key);
  cout << aes_128_gcm_decrypt(ciphertext, key) << endl;
}
