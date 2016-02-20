#include "gtest/gtest.h"
#include "cppqc.h"
#include <algorithm>
#include <iterator>
#include <sstream>

template <typename Collection>
void selection_sort(Collection &c, bool make_mistakes = false)
{
  auto b = std::begin(c)
    ,  e = std::end(c);

  make_mistakes && b != e ? ++b : b;
  for (auto i = b; i != e; ++i) {
    std::swap(*(std::min_element(i, e)), *i);
  }
}

struct PropTestSort: cppqc::Property<std::vector<int>>
{
  bool check(const std::vector<int> &v) const override
  {
    std::vector<int> v_copy(v);
    selection_sort(v_copy, false);
    return std::is_sorted(std::begin(v_copy), std::end(v_copy));
  }

  std::string name() const override
  {return "Sorting should be sorted";}

  std::string classify(const std::vector<int> &v) const override
  {return (std::ostringstream() << "size " << v.size()).str();}

  bool trivial(const std::vector<int> &v) const override
  {return v.empty() || v.size() == 1;}
};

int main(void)
{
  cppqc::quickCheckOutput(PropTestSort());
}
