#include "config.hpp"
#include "setup.hpp"
#include <fstream>
#include <iostream>
#include <utility>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
using namespace std;
using namespace boost;
using namespace cipher;

int main(int, const char * const*) {
  const string DICT_ARCHIVE = "dictionaries.txt";
  vector<Dictionary> dictionaries = analyze_dictionaries(RESOURCE());
  ofstream out(DICT_ARCHIVE);
  archive::text_oarchive oa(out);
  oa & dictionaries; // TODO: oa << dic[0:size-2] << dic[-1]
}
