#ifndef CRYPTO_PROJECT_1_KEY_YANG_LIU_0738
#define CRYPTO_PROJECT_1_KEY_YANG_LIU_0738
#include "rand_gen.hpp"
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

  static const std::size_t KEY_LEN = 103;

  // suppose alphabet is consecutive in its order
  constexpr static const char alpha = 'a';
  constexpr static const char omega = 'z';
  constexpr static const std::size_t alphabet_len = omega - alpha + 1;

  constexpr static const int slots[] {
      8 /* a */, 1 /* b */, 3 /* c */, 4 /* d */, 13 /* e */, 2 /* f */,
      2 /* g */, 6 /* h */, 7 /* i */, 1 /* j */, 1 /* k */, 4 /* l */,
      2 /* m */, 7 /* n */, 8 /* o */, 2 /* p */, 1 /* q */, 6 /* r */,
      6 /* s */, 9 /* t */, 3 /* u */, 1 /* v */, 2 /* w */, 1 /* x */,
      2 /* y */, 1 /* z */,
  };

  /// start index of a letter within the whole array
  // Ex: base['b' - 'a'] = 8; base['c' - 'a'] = 9
  static std::array<int, alphabet_len> base;

  /********************************************************************************
  * trivial details of generating base indices
  ********************************************************************************/
  namespace {
    template <int ch> struct base_of {
      static const int value = slots[ch - alpha] + base_of< ch-1 >::value;
    };
    template <> struct base_of< alpha > {
      static const int value = slots[0];
    };

    template <int i> struct gen_base {
      static int generate() {
        base[ i - alpha ] = base_of< i-1 >::value;
        return gen_base< i-1 >::generate();
      }
    };
    template <> struct gen_base< alpha > {
      static int generate() {
        base[0] = 0;
        return 0;
      }
    };
    static int ___trash = gen_base< omega >::generate();

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
      std::array<int, KEY_LEN> copied = codes;
      std::shuffle(std::begin(copied), std::end(copied), engine);
      _permutation = std::move(copied);
    }

    int code(char c, int idx) const {
      BOOST_ASSERT(idx < slots[c - alpha]);
      return _permutation[ base[c - alpha] + idx ];
    }

    inline std::array<int, KEY_LEN> key() const {return _permutation;}

  }; /* class Key */

} /* cipher */

#endif /* CRYPTO_PROJECT_1_KEY_YANG_LIU_0738 */
