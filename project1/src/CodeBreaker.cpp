#include "setup.hpp"
#include "key_basic.hpp"
#include "CodeBreaker.hpp"
#include <iostream>
#include <algorithm>
#include <boost/assert.hpp>
using namespace std;

namespace cipher {
  namespace detail {
    struct Decryptor {
      map<int, char> invert_key;
      map<int, size_t> use_count;

      bool apply_mapping(const string &plain, const vector<int> &numbers) {
        BOOST_ASSERT(plain.length() == numbers.size());
        auto endstr = plain.end();
        auto istr = plain.begin();
        auto ikey = numbers.begin();
        while (istr != endstr) {
          char ch = *istr++;
          int code = *ikey++;
          if (invert_key[code] == char()) {
            invert_key[code] = ch;
          } else if (invert_key[code] != ch) {
            revoke_mapping(vector<int>(numbers.begin(), ikey-1));
            return false;
          }
          use_count[code]++;
        }
        return true;
      }

      void revoke_mapping(const vector<int> &numbers) {
        for (auto code : numbers) {
          BOOST_ASSERT(use_count[code] > 0); // can be guaranteed logically
          if (--use_count[code] == 0) invert_key.erase(code);
        }
      }

      string dec(vector<int> codes) {
        ostringstream out;
        for (int k : codes) out << invert_key[k];
        return out.str();
      }

    }; /* Decryptor */

  } /* detail */
  detail::Decryptor decryptor;

  bool crack(const Dictionary &dict, const vector<vector<int> > &codes,
             vector<vector<int> >::iterator it) {
    if (it == codes.end()) return true;
    const vector<int> &numbers = *it;
    size_t size = numbers.size();
    for (const string &plain : dict.dict_map.at(size)) {
      if (decryptor.apply_mapping(plain, numbers)) {
        if (crack(dict, codes, it+1)) {
          return true;
        } else {
          decryptor.revoke_mapping(numbers);
        }
      }
    }
    return false;
  }

  vector<string> CodeBreaker::_solve(const vector<string> &ciphers) {
    decryptor = detail::Decryptor();
    // parse ciphers => a ciphertext word == a vector of int
    vector<vector<int>> codes;
    transform(begin(ciphers), end(ciphers), back_inserter(codes), [](string cipher) {
        vector<int> numbers;
        vector<string> num_strs = split(cipher, ",");
        transform(begin(num_strs), end(num_strs), back_inserter(numbers), [](string num_str) {
            return atoi(num_str.c_str());
          });
        return move(numbers);
      });

    for (Dictionary dict : dictionaries) {
      vector<vector<int> > copied(codes);
      for (auto numbers : copied) {
        // this dictionary doesn't contain any word of that size, skip
        if (dict.priority[numbers.size()] == 0) goto NEXT;
      }
      sort(begin(copied), end(copied), [&](vector<int> a, vector<int> b) {
          return dict.priority[a.size()] <  dict.priority[b.size()];
        });

      if (crack(dict, copied, copied.begin())) break;
    NEXT: ;
    }

    // cause it's guaranteed there is a correct answer, could skip correctness checking
    vector<string> plains;
    for (auto numbers : codes) plains.push_back(decryptor.dec(numbers));
    return move(plains);
  }

} /* cipher */
