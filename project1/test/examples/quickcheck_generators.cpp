#include <vector>
#include <type_traits>
#include <boost/random/uniform_int_distribution.hpp>
#include "cppqc.h"
#include <iostream>
using namespace std;
using namespace cppqc;

struct Gen_choose : Property<int> {
  Gen_choose(): Property(choose(1, 10)) {}

  bool check(const int &i) const override {
    return 1 <= i && i <= 10;
  }
};

struct Gen_elements :Property<int> {
  // only accept initializer_list, can't accept other containers :(
  Gen_elements(): Property(elements({1,2,3})) {}

  bool check(const int &i) const override {
    return (i == 1) || (i == 2) || (i == 3);
  }
};

template <typename C, typename T>
struct GenericElementsGenerator {
  GenericElementsGenerator(C container):
    _container(vector<typename C::value_type>(begin(container), end(container))) {}

  virtual ~GenericElementsGenerator() {}

  T unGen(RngEngine & rng, size_t size) {
    return sample(_container, rng, size);
  }

  vector<T> shrink(const T&) {return vector<T>();}

private:
  vector<typename C::value_type> _container;

  template <typename S>
  typename std::enable_if<std::is_same<typename S::value_type, T>::value, T>::type
  sample(S container, RngEngine &rng, size_t size) {
    //cout << "[SEE: base]" << endl;
    return take(container, rng, size);
  }

  template <typename S>
  typename std::enable_if<!std::is_same<typename S::value_type, T>::value, T>::type
  sample(S container, RngEngine &rng, size_t size) {
    //cout << "[SEE: recursive]" << endl;
    return sample(take(container, rng, size), rng, size);
  }

  template <typename S>
  auto take(S container, RngEngine &rng, size_t) -> typename S::value_type {
    boost::uniform_int<size_t> uni(0, _container.size() - 1);
    return container[uni(rng)];
  }

};

vector<vector<int>> makeMat() {
  vector<vector<int>> con {{1,2,3}};
  return con;
}

vector<vector<vector<int>>> makeTri() {
  vector<vector<vector<int>>> tri;
  tri.push_back(makeMat());
  return tri;
}

struct Gen_elements_from_container :Property<int> {
  //container.push_back(one);
  //Gen_elements_from_container(): Property(GenericElementsGenerator<vector<vector<vector<int>>>, int>(con)) {}
  //Gen_elements_from_container(): Property(GenericElementsGenerator<vector<vector<int>>, int>(makeMat())) {}
  Gen_elements_from_container(): Property(GenericElementsGenerator<vector<vector<vector<int>>>, int>(makeTri())) {}

  bool check(const int &i) const override {
    return 1 <= i && i <= 3;
  }
};

int main() {
  quickCheckOutput(Gen_choose());
  quickCheckOutput(Gen_elements());
  quickCheckOutput(Gen_elements_from_container());
}
