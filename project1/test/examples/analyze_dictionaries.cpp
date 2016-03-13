#include <iostream>
#include "config.hpp"
#include "setup.hpp"
using namespace std;
using namespace cipher;

template <typename T>
ostream& operator<<(ostream &o, const vector<T> &v) {
  o << "[ ";
  for (auto e : v) o << e << ", ";
  o << " ]";
  return o;
}

int main(void) {
  cout << "Have a look at dictionaries..." << endl;
  vector<words> dictionaries = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));
  for (words dict : dictionaries) {
    cout << dict << endl;
  }

  cout << endl << "Analyzing dictionaries..." << endl;
  vector<Dictionary> dicts = analyze_dictionaries(RESOURCE());
  size_t dict_no = 1;
  for (Dictionary dict : dicts) {
    cout << "Dictionary #" << dict_no++ << endl;
    for (auto entry : dict.dict_map) {
      size_t size = entry.first;
      cout << size << "(" << dict.priority[size] <<")" << " => [";
      for (string word : entry.second) {
        cout << word << "(" << fault_tolerances[word] << "), ";
      }
      cout << "]" << endl;
    }
    cout << endl;
  }
}
