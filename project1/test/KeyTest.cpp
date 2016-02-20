#include "gtest/gtest.h"
#include "cppqc.h"
#include "Key.hpp"
#include <numeric>
#include <random>
#include <algorithm>
using namespace std;
using namespace cipher;
using namespace cppqc;

/********************************************************************************
* Fixture
********************************************************************************/
static array<int, KEY_LEN> codes;
auto engine = default_random_engine{};

class KeyTest: public ::testing::Test {
protected:
  void SetUp() {
    for (size_t i = 0; i < KEY_LEN; i++) codes[i] = i;
  }
};


/********************************************************************************
* Basic Pre-conditions
********************************************************************************/
TEST_F (KeyTest, AlphabetRelatedSize) {
  EXPECT_EQ(alphabet_len, sizeof(slots) / sizeof(int));
  EXPECT_EQ(alphabet_len, base.size());
}

TEST_F (KeyTest, FrequencySumUpTo_KEY_LEN) {
  EXPECT_EQ((int)KEY_LEN, accumulate(begin(slots), end(slots), 0));
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

template <>
struct ArbitraryImpl<Key> {
  static const typename Arbitrary<Key>::unGenType unGen;
  static const typename Arbitrary<Key>::shrinkType shrink;
};

const typename Arbitrary<Key>::unGenType ArbitraryImpl<Key>::unGen = [](RngEngine &, size_t) {
  array<int, KEY_LEN> copied = codes;
  shuffle(begin(copied), end(copied), engine);
  return Key(copied);
};

const typename Arbitrary<Key>::shrinkType ArbitraryImpl<Key>::shrink = [](Key){
  return vector<Key>();
};


/********************************************************************************
* Properties
********************************************************************************/
#define RUN_QUICK_CHECK(property) TEST_F (KeyTest, property) \
  {EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(property()).result);}

struct KeyIsAPermutation: Property<Key> {
  bool check(const Key &k) const override {
    array<int, KEY_LEN> copied = k.key();
    sort(begin(copied), end(copied));
    return codes == copied;
  }
};
RUN_QUICK_CHECK(KeyIsAPermutation)

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
RUN_QUICK_CHECK(KeyDistNotBias)

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
    sort(begin(touched), end(touched));
    return touched == codes;
  }
};
RUN_QUICK_CHECK(KeySubIndexCoversAllKeys)
