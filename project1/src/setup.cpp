#include "setup.hpp"
#include "key_basic.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <map>
using namespace std;

namespace cipher {

  vector<string> split(const string &s, const string &sep) {
    vector<string> words;
    size_t pos;
    string str(s);
    while ((pos = str.find(sep)) != string::npos) {
      words.push_back(str.substr(0, pos));
      str.erase(0, pos + sep.length());
    }
    return move(words);
  }


  vector<vector<string>> load_dictionaries(const string &path) {
    ifstream file(path);
    vector<vector<string>> dictionaries;
    if (!file.is_open()) {return dictionaries;}

    string line;
    while (getline(file, line)) {
      istringstream in(line);
      int NO = 0; in >> NO;
      if (NO == 0) continue;

      getline(file, line);
      dictionaries.push_back(split(line));
    }
    return move(dictionaries);
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
  inline int fault_tolerance(const string &word) {
    int sum = 1;
    for (auto letter : word) sum *= slot_of(letter);
    return sum;
  }

  Dictionary parse_dictionary(const words &dictionary) {
    map<size_t, words> dict_map;
    for (string word : dictionary) {
      size_t size = word.length();
      dict_map[size].push_back(word);
    }

    vector<size_t> word_lengthes;
    // sort each sub dictionary by fault tolerance
    for (auto entry : dict_map) {
      word_lengthes.push_back(entry.first);
      words &sub_dict = entry.second;
      sort(begin(sub_dict), end(sub_dict), [](string a, string b) {
          return fault_tolerance(a) < fault_tolerance(b);
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
    for (int i = 0; i < size; i++) priority[word_lengthes[i]] = i;
    return Dictionary(move(dict_map), move(priority));
  }

  // this function combine them all
  vector<Dictionary> parse_dictionaries(const string &path) {
    vector<Dictionary> maps;
    vector<words> dictionaries = load_dictionaries(path);
    for (words dict : dictionaries) maps.push_back(parse_dictionary(dict));
    return move(maps);
  }

} /* cipher */
