#include "config.hpp"
#include "setup.hpp"
#include "testing.hpp"
#include "Key.hpp"
#include "Scheduler.hpp"
#include "CodeBreaker.hpp"
#include <iostream>
#include <ctime>
#include <fstream>
#include <future>
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
      ofstream log(RESOURCE("break.log"));
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

  bool expect() const override {return false;}

  bool check(const Scheduler &s, const Key &k, const vector<string> &words) const override {
    vector<string> ciphers;
    for (string w : words) ciphers.push_back(join(s.enc(w, k), ","));
    string ciphertext = join(ciphers);

    atomic<bool> stop{false};
    CodeBreaker breaker;
    auto future_receive = async([&]() {
        return breaker.solve(ciphertext, dicts);
      });

    future_status status;
    bool correct = false;
    auto start = clock();
    ofstream log(RESOURCE("english.log"));
    string plaintext = join(words);
    const size_t TIME_LIMIT = 120;

    status = future_receive.wait_for(chrono::seconds(TIME_LIMIT));
    if (status == future_status::timeout) {
      breaker.stop = true;
      cout << "Timeout! abort." << endl;
      log << TIME_LIMIT << "," << plaintext << endl;
    } else if (status == future_status::ready) {
      auto end = clock();
      correct = future_receive.get() == plaintext;
      if (correct) {
        cout << "crack in " << (end - start) / CLOCKS_PER_SEC << " seconds " << endl;
        log << (end - start) / CLOCKS_PER_SEC << "," << plaintext << endl;
        log.flush();
      }
    }
    return true; // always move on even fail
  }

};

TEST(CodeBreakTest, EnglishDictionary) {
  EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(EnglishDictionary()).result);
}
