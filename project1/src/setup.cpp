#include "setup.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
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

}
