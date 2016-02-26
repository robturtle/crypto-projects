#include "CodeBreaker.hpp"
using namespace std;

int main(void) {
  string input;
  getline(cin, input);
  cout << '\'' << input << '\'' << endl;
  cout << cipher::CodeBreaker().solve(input) << endl;
}
