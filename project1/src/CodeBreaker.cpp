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

      string dec(const vector<int> &codes) {
        ostringstream out;
        for (int k : codes) out << invert_key[k];
        return out.str();
      }

    }; /* Decryptor */

  } /* detail */
  detail::Decryptor decryptor;

  bool CodeBreaker::crack(const Dictionary &dict, const vector<vector<int> > &codes,
             vector<vector<int> >::iterator it) {
    if (stop) return false;
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

  inline unsigned long long choose(unsigned long long from, unsigned long long pick) {
    if (pick > from) pick = from;
    else if (pick > from / 2) pick = from - pick;
    unsigned long long count = 1;
    for (unsigned int i = 0, multiple = from; i < pick; i++, multiple--)
      count *= multiple;
    for (unsigned int i = pick; i > 1; i--)
      count /= i;
    return count;
  }

  vector<string> CodeBreaker::_solve(const vector<string> &ciphers, const vector<Dictionary> &dictionaries) {
    decryptor = detail::Decryptor();
    // parse ciphers => a ciphertext word == a vector of int
    vector<vector<int>> codes;
    map<size_t, vector<vector<int>>> codes_by_word_length;
    map<size_t, size_t> num_of_word_length;
    vector<size_t> word_lengthes;
    for (string strings : ciphers) {
      vector<int> numbers;
      vector<string> num_strs = split(strings, ",");
      transform(begin(num_strs), end(num_strs), back_inserter(numbers), [](string num_str) {
          return atoi(num_str.c_str());
        });
      size_t sz = numbers.size();
      codes_by_word_length[sz].push_back(numbers);
      codes.push_back(numbers);
      if (num_of_word_length[sz] == 0) word_lengthes.push_back(sz);
      num_of_word_length[sz] += 1;
    }

    for (Dictionary dict : dictionaries) {
      vector<vector<int>> copied;
      copied.clear();
      for (auto sz : word_lengthes) {
        // this dictionary doesn't contain any word of that size, skip
        if (dict.priority[sz] == 0) goto NEXT;
      }

      sort(begin(word_lengthes), end(word_lengthes), [&](size_t a, size_t b) {
          auto ca = choose(dict.dict_map[a].size(), a);
          auto cb = choose(dict.dict_map[b].size(), b);
          if (ca == cb) {
            return dict.priority[a] < dict.priority[b];
          } else {
            return ca < cb;
          }
        });

      for (size_t sz : word_lengthes) {
        copy(begin(codes_by_word_length[sz]), end(codes_by_word_length[sz]),
             back_inserter(copied));
      }

      if (crack(dict, copied, copied.begin())) break;
    NEXT: ;
    }

    // cause it's guaranteed there is a correct answer, could skip correctness checking
    vector<string> plains;
    for (auto numbers : codes) plains.push_back(decryptor.dec(numbers));
    return move(plains);
  }

} /* cipher */
