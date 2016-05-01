#include <iostream>
#include "crypto.hpp"
using namespace std;
using namespace crypto;

int main(void) {
  cout << gen_key_16() << endl;
}
