#include "config.hpp"
#include "setup.hpp"
#include "Scheduler.hpp"
#include <random>
using namespace std;
using namespace cipher;

int main(int, char**) {

  vector<words> dictionaries = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));
  size_t dict_idx = uniform_int_distribution<size_t>(0, dictionaries.size() - 1)(engine);
  const words &dict = dictionaries[dict_idx];

  auto uni = uniform_int_distribution<size_t>(0, dict.size() - 1);
  const size_t word_num = 100;
  vector<string> words;
  for (size_t i = 0; i < word_num; i++) {
    words.push_back(dict[uni(engine)]);
  }

  Key key;
  string ciphertext = cipher::uniform.encode(words, key);

  cerr << "Generate plaintext:" << endl
       << join(words) << endl;

  cout << ciphertext << endl;

}
