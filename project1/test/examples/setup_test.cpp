#include "setup.hpp"
#include "config.hpp"
#include <iostream>
using namespace std;
using namespace cipher;

int main(void) {
  vector<vector<string>> dictionaries = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));
  for (auto dic : dictionaries) {
    cout << "Dictionary:" << endl;
    for (auto word : dic) {
      cout << word << " | ";
    }
    cout << endl << endl;
  }
}
