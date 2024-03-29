#include "setup.hpp"
#include "key_basic.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <map>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
using namespace std;
using namespace boost;

namespace cipher {

  vector<string> split(const string &s, const string &sep) {
    vector<string> words;
    size_t pos;
    string str(s);
    while ((pos = str.find(sep)) != string::npos) {
      words.push_back(str.substr(0, pos));
      str.erase(0, pos + sep.length());
    }
    if (!str.empty()) words.push_back(str.substr(0, str.length()));
    return words;
  }


  string basename(const string &path) {
    size_t pos = path.find_last_of('/');
    return path.substr(0, pos);
  }


  vector<vector<string>> load_dictionaries(const string &path) {
    ifstream file(path);
    vector<vector<string>> dicts;
    if (!file.is_open()) {return dicts;}

    string line;
    while (getline(file, line)) {
      istringstream in(line);
      int NO = 0; in >> NO;
      if (NO == 0) continue;

      getline(file, line);
      dicts.push_back(split(line));
    }
    return dicts;
  }

  // this function only split a single dictionary
  /*
    We could try to start it from the easiest one, i.e. the one who has the least candidates.

    We split the dictionary by its word's length, take the 1st dictionary for example, we will got something
    like this:
    ```ruby
    {
    7=>["sconced", "coastal", "anthrax", "polypus", "kayaker", "william", "blunted", "cumquat", "aniline", "academe", "rebukes", "swagmen"],
    5=>["pouch", "colly", "lacer"],
    6=>["bogart", "lights", "philip", "resins", "kevils", "dredge"],
    12=>["nonexplosive", "outstripping", "underbidding", "corespondent"],
    8=>["shriller", "trembler", "research", "parapets", "breveted", "cyanosis", "obstacle"],
    11=>["nightshirts", "unresistant", "concertizes"],
    13=>["editorializer", "industrialist"],
    10=>["gratuitous", "almightily", "endoscopes", "ostensible"],
    9=>["pyrometer", "scoutings", "agileness", "toothpick", "atavistic"],
    14=>["psychoneurosis"],
    4=>["hake"],
    17=>["incompressibility"],
    15=>["nondistribution", "plenipotentiary"],
    1=>["p"]
    }
    ```

    As we can see, candidates of the word length 1, 4, 14, and 17 are only of size 1. That is to say, once we came
    across ciphertext with such length, we can immediately get its corresponding plaintext word and the partial
    key mapping of. The we could go through words with 2 candidates, words with 3 candidates, and so on.

    And for which have the same amount of candidates, of cause we should start checking from the longest one for
    it can bind more key mappings in the early stage, hence we might detect the mapping conflicts earlier.

    Based on the knowledge learned from the dictionary, let's reconsider the key space. We know that the whole
    length of the ciphertext is around 500. Considering it's built up with 72 word of each has the length of 7.
    The size of the key space will be |map[7]|^72 = 12^72. It's impractical to iterate from it. But if we go through
    the depth-first search, we can easily prune based on the pre-binded key mappings.

    In order to make the conflict appear as soon as possible, we could sort the sub-dictionaries by letting the word
    with the least fault tolerance take the beginning position.

    The fault tolerance should be calculated as: FT(word) = Π (slot number of each letter).

    For example, the fault tolerance of "aet" = 8 * 13 * 9 = 936
  */
  inline unsigned long long fault_tolerance(const string &word) {
    unsigned long long sum = 1;
    for (auto letter : word) sum *= slot_of(letter);
    return sum;
  }

  map<string, double> fault_tolerances;

  Dictionary analyze_dictionary(const words &dictionary) {
    map<size_t, words> dict_map;
    for (const string &word : dictionary) {
      size_t size = word.length();
      dict_map[size].push_back(word);
      fault_tolerances[word] = fault_tolerance(word);
    }

    vector<size_t> word_lengthes;
    // sort each sub dictionary by fault tolerance
    for (auto &entry : dict_map) {
      word_lengthes.push_back(entry.first);
      words &sub_dict = entry.second;
      sort(begin(sub_dict), end(sub_dict), [](string a, string b) {
          return fault_tolerances[a] < fault_tolerances[b];
        });
    }

    // sort the word_lengthes by
    // 1. size of the sub dictionary (Ascending)
    // 2. word length (Descending)
    sort(begin(word_lengthes), end(word_lengthes), [&](size_t a, size_t b) {
        if (dict_map[a].size() == dict_map[b].size()) {
          return a > b;
        } else {
          return dict_map[a].size() < dict_map[b].size();
        }
      });
    map<size_t, int> priority;
    int size = word_lengthes.size();
    // priority[x] = 0 means non existed
    for (int i = 1; i <= size; i++) priority[word_lengthes[i-1]] = i;
    return Dictionary(move(dict_map), move(priority));
  }

  words load_english_words(const string &path) {
    const string ENGLISH_DICT = "/english_words.txt";
    ifstream in(path + ENGLISH_DICT);
    words english_words;
    copy(istream_iterator<string>(in),
         istream_iterator<string>(),
         back_inserter(english_words));
    return english_words;
 }

  // this function combine them all
  vector<Dictionary> analyze_dictionaries(const string &path) {
    const string SMALL_DICTS = "/plaintext_dictionary.txt";
    vector<Dictionary> maps;
    vector<words> dictionaries = load_dictionaries(path + SMALL_DICTS);
    for (const words &dict : dictionaries) maps.push_back(analyze_dictionary(dict));
    maps.push_back(analyze_dictionary(load_english_words(path)));
    return maps;
  }

  void load_dicts(const std::string &path, std::vector<Dictionary> &dicts) {
    ifstream in(path);
    archive::text_iarchive ia(in);
    ia & dicts;
  }
} /* cipher */
