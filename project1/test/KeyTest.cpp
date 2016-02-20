#include "gtest/gtest.h"
#include "Key.hpp"
#include <numeric>
using namespace std;
using namespace cipher;

TEST (KeyTest, AlphabetRelatedSize) {
  EXPECT_EQ(alphabet_len, sizeof(slots) / sizeof(int));
  EXPECT_EQ(alphabet_len, base.size());
}

TEST (KeyTest, FrequencySumUpTo_KEY_LEN) {
  EXPECT_EQ(KEY_LEN, accumulate(begin(slots), end(slots), 0));
}
