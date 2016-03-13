#include "config.hpp"
#include "setup.hpp"
#include "testing.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
#include "CodeBreaker.hpp"
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;
using namespace cppqc;
using namespace cipher;

// It will generate plaintext from one of the dictionaries,
// encode it into ciphertext,
// and challenge the code breaker

static vector<vector<string>> plaintext_sets = load_dictionaries(RESOURCE("plaintext_dictionary.txt"));

/********************************************************************************
 * Generator of ciphertext
 ********************************************************************************/
static const size_t WORD_NUM_MIN = 80;
static const size_t WORD_NUM_MAX = 100;

struct PlaintextGenerator {
  vector<string> unGen(RngEngine &rng, size_t) {
    vector<string> words;
    vector<string> dictionary = random_take(plaintext_sets, rng);
    const size_t CHAR_NUM_MAX = 500;
    size_t char_num =0;
    while (char_num < CHAR_NUM_MAX) {
      string word = random_take(dictionary, rng);
      char_num += word.length();
      words.push_back(word);
    }
    return move(words);
  }

  vector<vector<string>> shrink(vector<string> /*words*/) {
    return vector<vector<string>>();
  }
};

struct CipherProperty: Property<Scheduler, Key, vector<string>> {
  CipherProperty(): Property(gen_scheduler,
                                Arbitrary<Key>(),
                                // pick one dictionary, and pick one word from it
                                PlaintextGenerator()) {}
};


/********************************************************************************
 * Properties
 ********************************************************************************/

// Dec(Enc(plaintext)) = plaintext
struct Correctness: CipherProperty {
  vector<Dictionary> dicts;

  Correctness() {
    load_dicts(RESOURCE("../src/dictionaries.txt"), dicts);
  }

  bool check(const Scheduler &s, const Key &k, const vector<string> &words) const override {
    vector<string> ciphers;
    for (string w : words) ciphers.push_back(join(s.enc(w, k), ","));
    string ciphertext = join(ciphers);

    auto start = clock();
    string inverted = CodeBreaker().solve(ciphertext, dicts);
    auto end = clock();

    string plaintext = join(words);
    bool correct = inverted == plaintext;

    // logging for further analysis
    if (correct) {
      // in microseconds
      ofstream log(RESOURCE("break.log"), ios_base::app);

      cout << "crack in " << (end - start) / CLOCKS_PER_SEC << " seconds" << endl;
      cout.flush();

      log << (end - start) * 1000000 /CLOCKS_PER_SEC << ','
          << plaintext << endl;
      log.flush();
    } else {
      cout << endl << "ciphertext: " << ciphertext << endl;
      cout << endl << "inverted: " << inverted << endl;
    }

    return correct;
  }
};
RUN_QUICK_CHECK(CodeBreakTest, Correctness)

struct EnglishWordPicker {
  vector<string> unGen(RngEngine &rng, size_t) {
    vector<string> words;
    vector<string> english_words = load_english_words(RESOURCE());
    const size_t CHAR_NUM_MAX = 500;
    size_t char_num =0;
    while (char_num < CHAR_NUM_MAX) {
      string word = random_take(english_words, rng);
      char_num += word.length();
      words.push_back(word);
    }
    return move(words);
  }

  vector<vector<string>> shrink(vector<string>) {
    return vector<vector<string>>();
  }
};

struct EnglishDictionary: public Correctness {
  EnglishDictionary() {
    plaintext_sets.clear();
    plaintext_sets.push_back(load_english_words(RESOURCE()));
  }
};
RUN_QUICK_CHECK(CodeBreakTest, EnglishDictionary)
