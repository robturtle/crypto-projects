#ifndef CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#define CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#include <vector>
#include <string>
#include <sstream>
#include <map>

namespace cipher {
  using words = std::vector<std::string>;
  
  extern words split(const std::string &, const std::string &sep = " ");

  template <typename T>
  std::string join(const std::vector<T> &words, const std::string &sep = " ") {
    std::ostringstream out;
    auto i = words.begin();
    auto end = words.end();
    out << *i++;
    for (; i != end; ++i) out << sep << *i;
    return move(out.str());
  }

  extern std::vector<words> load_dictionaries(const std::string &path);

  extern std::map<int, words> split_dictionary(std::vector<std::string> dictionary);
  extern std::vector<std::map<int, words>> split_dictionaries(const std::string &path);
}

#endif // CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
