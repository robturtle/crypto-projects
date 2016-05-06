#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(void) {
  string ciphertext;
  getline(cin, ciphertext);
  Rsa rsa { "", "rsa.key" };
  cout << rsa.decrypt(ciphertext) << endl;
}
