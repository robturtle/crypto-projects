#include <fstream>
#include "setup.hpp"
#include "CodeBreaker.hpp"
using namespace std;
using namespace cipher;
using namespace boost;

int main(int, char **argv) {
  vector<Dictionary> dicts;
  load_dicts(cipher::basename(argv[0]) + string("/dictionaries.txt"), dicts);

  string input;
  getline(cin, input);
  cout << cipher::CodeBreaker().solve(input, dicts) << endl;
}
