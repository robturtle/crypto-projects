#ifndef CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#define CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#include <vector>
#include <string>
#include <sstream>

namespace cipher {

  extern std::vector<std::string> split(const std::string &, const std::string &sep = " ");

  template <typename T>
  std::string join(const std::vector<T> &words, const std::string &sep = " ") {
    std::ostringstream out;
    auto i = words.begin();
    auto end = words.end();
    out << *i++;
    for (; i != end; ++i) out << sep << *i;
    return move(out.str());
  }

  extern std::vector<std::vector<std::string>> load_dictionaries(const std::string &path);

}

#endif // CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
