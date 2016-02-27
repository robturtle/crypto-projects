#ifndef CRYPTO_PROJECT_1_KEY_YANG_LIU_0738
#define CRYPTO_PROJECT_1_KEY_YANG_LIU_0738
#include "key_basic.hpp"
#include "testing.hpp"
#include <array>
#include <boost/assert.hpp>
#include <random>

namespace cipher {
  /**
   * Key of a cryptosystem specified in
   * https://newclasses.nyu.edu/portal/site/7da14569-3e27-4ad7-8d2e-3cd148d5cc7d/page/bf131ccd-e328-4619-b0f8-062a8da0a18f
   *
   * The plaintext is a space-separated sequence of words from the
   * English dictionary (the sentence may not be meaningful). The key
   * is a map from each English alphabet (lower-case) letter to a list
   * of numbers randomly chosen between 0 and 102, where the length of
   * this list is the (rounded) letter’s frequency in English text, as
   * defined in the table below. The ciphertext is a space-separated
   * sequence of encryptions of words, where each word is encrypted as
   * a comma-separated list of numbers between 0 and 102, and these
   * numbers are computed using the table below.
   */

  namespace {
    static std::array<int, KEY_LEN> gen_codes() {
      std::array<int, KEY_LEN> ary;
      for (std::size_t i = 0; i < KEY_LEN; i++) ary[i] = i;
      return std::move(ary);
    }
  }

  // codes = [0, 1, 2, ..., 102]
  static const std::array<int, KEY_LEN> codes = gen_codes();

  /********************************************************************************
  * key instance
  ********************************************************************************/
  class Key {

  private:
    std::array<int, KEY_LEN> _permutation;

  public:
    Key(const std::array<int, KEY_LEN> &permutation):_permutation(permutation) {}

    Key() {
      std::default_random_engine engine(std::random_device{}());
      std::array<int, KEY_LEN> copied = codes;
      std::shuffle(std::begin(copied), std::end(copied), engine);
      _permutation = std::move(copied);
    }

    int code(char c, int idx) const {
      BOOST_ASSERT(alpha <= c && c <= omega);
      BOOST_ASSERT(0 <= idx && idx < slot_of(c));
      return _permutation[ base[c - alpha] + idx ];
    }

    char plain(int code) const {
      BOOST_ASSERT(0 <= code && code < (int)KEY_LEN);
      int index = std::find(_permutation.begin(), _permutation.end(), code) - _permutation.begin();
      char ch = alpha;
      for (int pos = 0; pos <= index; pos += slot_of(ch++)) {}
      return ch-1;
    }

    inline std::array<int, KEY_LEN> key() const {return _permutation;}

  }; /* class Key */

  std::ostream& operator<<(std::ostream &o, const Key &k) {
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

} /* cipher */

static cipher::Key gen_key(cppqc::RngEngine&, std::size_t) {return std::move(cipher::Key());}
REGISTER_ARBITRARY(cipher::Key, gen_key, cppqc::shrinkNothing);


#endif /* CRYPTO_PROJECT_1_KEY_YANG_LIU_0738 */
