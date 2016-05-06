#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

void USAGE(const string& program) {
  cout << "USAGE: " << program << " RSA_publicKey plaintext" << endl;
  exit(1);
}

int main(int argc, const char * const argv[]) {
  if (argc != 3) { USAGE(string{ argv[0] }); }
  string pubKey { argv[1] };
  string plaintext { argv[2] };
  Rsa rsa;
  rsa.readPublicKey(pubKey);
  cout << rsa.encrypt(plaintext) << endl;
}
