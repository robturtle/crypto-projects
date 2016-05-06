#include <string.h>
#include <iostream>
#include "base64.h"
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(void) {
  string pubFname { "rsa.pub" };
  string keyFname { "rsa.key" };
  // if file not exists, generate new and store then into such files,
  // otherwise load directly from the file
  Rsa rsa(pubFname, keyFname);
  cout << "Generate/Load key..." << endl;
  rsa.write(cout, cout);

  // Rsa::readPlaintext will automatically trunk over long input
  cout << "Message to encrypt (end with EOF): ";
  string msg { rsa.readPlaintext(cin) };
  cout << msg << endl;

  string ciphertext = rsa.encrypt(msg);
  cout << "Encrypted message: " << ciphertext << endl;

  string plaintext  = rsa.decrypt(ciphertext);
  cout << "Decrypted message: " << plaintext << endl;

  return 0;
}
