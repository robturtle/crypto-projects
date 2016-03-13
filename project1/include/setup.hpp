#ifndef CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#define CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace cipher {
  using words = std::vector<std::string>;

  extern words split(const std::string &, const std::string &sep = " ");

  template <typename T>
  std::string join(const std::vector<T> &words, const std::string &sep = " ") {
    if (words.empty()) return "";
    std::ostringstream out;
    auto i = words.begin();
    auto end = words.end();
    out << *i++;
    for (; i != end; ++i) out << sep << *i;
    return move(out.str());
  }

  extern std::string basename(const std::string &path);
  extern std::vector<words> load_dictionaries(const std::string &path);

  // group words by its length
  // and provide information of priority
  struct Dictionary {
    Dictionary() {}
    Dictionary(const std::map<std::size_t, words> &dict_map, const std::map<std::size_t, int> &priority)
      : dict_map(dict_map), priority(priority) {}
    std::map<std::size_t, words> dict_map;
    // priority[x] = 0 means non existed
    std::map<std::size_t, int> priority; // word length => priority, the smaller value, the higher priority

    template <typename Archive>
    void serialize(Archive& archive, const unsigned int) {
      archive & dict_map;
      archive & priority;
    }
  }; /* Dictionary */

  extern Dictionary analyze_dictionary(const words &dictionary);
  extern std::vector<Dictionary> analyze_dictionaries(const std::string &path);
  extern void load_dicts(const std::string &path, std::vector<Dictionary> &dicts);
  extern words load_english_words(const std::string &path);

} /* cipher */

#endif // CRYPTO_PRJECT_1_YANG_LIU_SETUP_0630
