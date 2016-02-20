#include "cppqc.h"

// This property tests that A*A > A holds.
//
// In general, this is not true, for instance, if A is zero.
// However, by using the "choose" generator, we can define that
// A is in a given interval (between 2 and 1000 in this example).
using namespace cppqc;
using namespace std;

// here we create a custom class
template <typename T>
struct Data {
  Data(T v): _v(v) {}
  bool operator==(const Data<T> &that) const {return _v == that._v;}
  T _v;
};

// must implement <<
template <typename T>
ostream& operator<<(ostream &o, const Data<T> &e)
{o << e._v; return o;}

// must provide a random variable generator unGen
// and a shrinking vector generator shrink
template <typename T>
struct ArbitraryImpl<Data<T>>
{
  static const typename Arbitrary<Data<T>>::unGenType unGen;
  static const typename Arbitrary<Data<T>>::shrinkType shrink;
};

// RngEngine: random generator engine
// size: bound of the domain
template <typename T>
const typename Arbitrary<Data<T>>::unGenType ArbitraryImpl<Data<T>>::unGen = [](RngEngine &rng, size_t size) {
  return Data<T>(T());
};

// make violated sample size smaller
// should have trivial case or otherwise will cause infinite loop
template <typename T>
const typename Arbitrary<Data<T>>::shrinkType ArbitraryImpl<Data<T>>::shrink = [](Data<T> e) {
  vector<Data<T>> ret {};
  return ret;
};


// original quick check
struct AxA_isGreaterThan_A : Property<Data<int>, Data<int>>
{
  static constexpr int MIN = 2;
  static constexpr int MAX = 1000;

  //AxA_isGreaterThan_A(): Property(choose(MIN, MAX)) {}

  bool check(const Data<int> &A, const Data<int> &B) const override
  {return A._v == B._v;} // fail if !=, pass if ==

  std::string name() const override
  {return "Custom class";}

};

int main()
{
  quickCheckOutput(AxA_isGreaterThan_A());
}
