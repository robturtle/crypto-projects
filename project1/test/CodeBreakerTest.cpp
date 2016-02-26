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
static const size_t WORD_NUM_MIN = 200;
static const size_t WORD_NUM_MAX = 200;

struct PlaintextGenerator {
  vector<string> unGen(RngEngine &rng, size_t) {
    vector<string> words;
    vector<string> dictionary = random_take(plaintext_sets, rng);
    cout << "Choosing dictionary of " << dictionary[0] << endl;
    size_t word_num = boost::uniform_int<size_t>(WORD_NUM_MIN, WORD_NUM_MAX)(rng);
    for (size_t i = 0; i < word_num; i++)
      words.push_back(random_take(dictionary, rng));
    return move(words);
  }

  vector<vector<string>> shrink(vector<string> /*words*/) {
    /*
    vector<vector<string>> shrinks;
    for (size_t size = words.size() / 2; size >= WORD_NUM_MIN; size /= 2) {
      vector<string> shrink(size);
      copy(words.begin(), words.begin() + size, back_inserter(shrink));
      shrinks.push_back(shrink);
    }
    return shrinks;
    */
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
  bool check(const Scheduler &s, const Key &k, const vector<string> &words) const override {
    vector<string> ciphers;
    for (string w : words) ciphers.push_back(join(s.enc(w, k), ","));
    string ciphertext = join(ciphers);
    cout << endl << "ciphertext: " << ciphertext << endl;

    auto start = clock();
    string inverted = CodeBreaker().solve(ciphertext);
    auto end = clock();
    cout << endl << "inverted: " << inverted << endl;

    string plaintext = join(words);
    bool correct = inverted == plaintext;

    // logging for further analysis
    if (correct) ofstream(RESOURCE("break.log"), ios_base::app) << (end - start)/CLOCKS_PER_SEC << ','
                                                                << plaintext << ','
                                                                << s.name << endl;

    return correct;
  }
};
RUN_QUICK_CHECK(CodeBreakTest, Correctness)
