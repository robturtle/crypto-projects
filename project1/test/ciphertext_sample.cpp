#include "config.hpp"
#include "setup.hpp"
#include "Scheduler.hpp"
#include <random>
using namespace std;
using namespace cipher;

int main(int argc, char**argv) {

  default_random_engine engine(random_device{}());

  vector<words> dictionaries = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));
  words dict;
  if (argc > 1 && string(argv[1]) == "english") {
    dict = load_english_words(RESOURCE());
  } else {
    size_t dict_idx = uniform_int_distribution<size_t>(0, dictionaries.size() - 1)(engine);
    dict = dictionaries[dict_idx];
  }

  auto uni = uniform_int_distribution<size_t>(0, dict.size() - 1);
  const size_t CHAR_NUM_MAX = 500;
  size_t char_num = 0;
  vector<string> words;
  while (char_num < CHAR_NUM_MAX) {
    string word = dict[uni(engine)];
    char_num += word.length();
    words.push_back(dict[uni(engine)]);
  }

  Key key;
  string ciphertext = cipher::uniform.encode(words, key);

  cerr << "Generate plaintext:" << endl
       << join(words) << endl;

  cout << ciphertext << endl;

}
