#include "config.hpp"
#include "setup.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
#include "CodeBreaker.hpp"
#include <sstream>
#include <fstream>
using namespace std;
using namespace cipher;

default_random_engine engine(random_device{}());
vector<words> vocabularies = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));

string pick(const words &vocabulary) {
  uniform_int_distribution<size_t> uni(0, vocabulary.size() - 1);
  return vocabulary[uni(engine)];
}
/**
 * return a new word based on the:
 * 1. word length distribution of dict
 * 2. letter distribution (defined by 103 slots)
 */
string new_word(const Dictionary &dict) {
  return "NOT IMPL";
}

/**
 * double size the words and keep the distribution of
 * 1. word length distribution
 * 2. letter distribution (defined by 103 slots)
 * not varied much
 */
void double_size(words &vocabulary) {}

size_t count_time_in_sec(const words &vocabulary, const string &log_path, size_t word_num = 100) {
  words plains;
  for (size_t i = 0; i < word_num; i++) plains.push_back(pick(vocabulary));
  string ciphertext = uniform.encode(plains, Key());
  auto start = clock();
  CodeBreaker().solve(ciphertext);
  auto end = clock();
  return (end - start) / CLOCKS_PER_SEC;
}

int main(int, char **argv) {

  // do experiments on each original dictionary
  size_t dict_idx = 0;
  for (words vocabulary : vocabularies) {
    dict_idx++;

    // Experiment 1
    ostringstream fname; fname << "exp1-dict" << dict_idx << ".csv";
    string path = RESOURCE( + fname.str()); // little bit hack here
    ofstream log(path);
    size_t time_in_sec = 0;
    do {
      time_in_sec = count_time_in_sec(vocabulary, path);
      log << time_in_sec << "," << vocabulary.size() << endl;
      double_size(vocabulary);
    } while (time_in_sec > 60 * 2);

    // Experiment 2
    const size_t word_length = 10;
    ostringstream fanme; fname << "exp2-dict" << dict_idx << ".csv";
    // ...
  }
}
