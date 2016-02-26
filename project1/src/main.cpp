#include "setup.hpp"
#include "CodeBreaker.hpp"
using namespace std;
using namespace cipher;

int main(int, char **argv) {
  string input;
  getline(cin, input);
  cout << cipher::CodeBreaker(basename(argv[0]) + "/plaintext_dictionary.txt").solve(input) << endl;
}
