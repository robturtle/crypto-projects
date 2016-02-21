#include "testing.hpp"
#include "Key.hpp"
#include <numeric>
#include <random>
#include <algorithm>
using namespace std;
using namespace cipher;
using namespace cppqc;

/********************************************************************************
* Basic Pre-conditions
********************************************************************************/
TEST (KeyTest, AlphabetRelatedSize) {
  ASSERT_EQ(alphabet_len, sizeof(slots) / sizeof(int));
  ASSERT_EQ(alphabet_len, base.size());
}

TEST (KeyTest, FrequencySumUpTo_KEY_LEN) {
  ASSERT_EQ((int)KEY_LEN, accumulate(begin(slots), end(slots), 0));
}


/********************************************************************************
* Generator of Key
********************************************************************************/
namespace cipher {
  ostream& operator<<(ostream &o, const Key &k) {
    char ch = alpha;
    int idx = 0;
    for (int slot : slots) {
      o << ch++ << ": ";
      auto key = k.key();
      for (int i = 0; i < slot; i++) {
        o << key[idx++] << ", ";
      }
      o << "\n";
    }
    return o;
  }
}

/********************************************************************************
* Properties
********************************************************************************/

// each number should appear exactly once
struct KeyIsAPermutation: Property<Key> {
  bool check(const Key &k) const override {
    array<int, KEY_LEN> copied = k.key();
    sort(begin(copied), end(copied));
    return codes == copied;
  }
};
RUN_QUICK_CHECK(KeyTest, KeyIsAPermutation)

// It's very unlikely that 2 identical keys were generated regarding to the
// key space 103!
struct KeyDistNotBias: Property<vector<Key>> {
  bool check(const vector<Key> &keys) const override {
    for (auto i = begin(keys); i < end(keys); ++i) {
      for (auto j = i + 1; j < end(keys); ++j) {
        if ((*i).key() == (*j).key()) return false;
      }
    }
    return true;
  }
};
RUN_QUICK_CHECK(KeyTest, KeyDistNotBias)

// iterating through ('a', 0), ('a', 1), ..., ('z', 0) should cover all keys
struct KeySubIndexCoversAllKeys: Property<Key> {
  bool check(const Key& k) const override {
    array<int, KEY_LEN> touched;
    int idx = 0;
    for (char c = alpha; c <= omega; c++) {
      int slot_num = slots[c - alpha];
      for (int i = 0; i < slot_num; i++) {
        touched[idx++] = k.code(c, i);
      }
    }
    if (idx != KEY_LEN) return false;
    sort(begin(touched), end(touched));
    return touched == codes;
  }
};
RUN_QUICK_CHECK(KeyTest, KeySubIndexCoversAllKeys)

struct KeyInvertCodeToCharCorrectly: Property<Key, char> {
  KeyInvertCodeToCharCorrectly(): Property(Arbitrary<Key>(), choose(alpha, omega)) {}

  bool check(const Key &k, const char &c) const override {
    std::uniform_int_distribution<int> uni(0, slot_of(c) - 1);
    int rand_index = uni(engine);
    return c == k.plain(k.code(c, rand_index));
  }

};
RUN_QUICK_CHECK(KeyTest, KeyInvertCodeToCharCorrectly)
