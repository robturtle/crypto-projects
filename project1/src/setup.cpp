#include "setup.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
using namespace std;

namespace cipher {

  vector<string> split(const string &s) {
    istringstream in(s);
    vector<string> words;
    copy(istream_iterator<string>(in),
         istream_iterator<string>(),
         back_inserter(words));
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

}
