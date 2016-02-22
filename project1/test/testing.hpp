#ifndef GTEST_QUICKCHECK_COORPERATE_YANG_LIU_0211
#define GTEST_QUICKCHECK_COORPERATE_YANG_LIU_0211
#include "gtest/gtest.h"
#include "cppqc.h"
#include <vector>
#include <type_traits>

// specify how a new data type varies and shrinks
// register it so the it can play as the type variable in QuickCheck
#define REGISTER_ARBITRARY(Class, gen, shrinker)                        \
  template <> struct cppqc::ArbitraryImpl<Class> {                      \
    static const typename cppqc::Arbitrary<Class>::unGenType unGen;     \
    static const typename cppqc::Arbitrary<Class>::shrinkType shrink;   \
  };                                                                    \
  const typename cppqc::Arbitrary<Class>::unGenType                     \
  cppqc::ArbitraryImpl<Class>::unGen = [](cppqc::RngEngine &rng, std::size_t size) { \
    return gen(rng, size);                                              \
  };                                                                    \
  const typename cppqc::Arbitrary<Class>::shrinkType                    \
  cppqc::ArbitraryImpl<Class>::shrink = [](Class c) {                   \
    return shrinker(c);                                                 \
  }


// wrap a quickcheck runner into a gtest test case
#define RUN_QUICK_CHECK(fixture, property) TEST (fixture, property)     \
  {EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(property()).result);}


#define RUN_QUICK_CHECK_F(fixture, property) TEST_F (fixture, property) \
  {EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(property()).result);}

// simple sample function
template <typename S>
typename S::value_type
random_take(S container, cppqc::RngEngine &rng) {
  boost::uniform_int<std::size_t> uni(0, container.size() - 1);
  return container[uni(rng)];
}


// cppqc::elements can only accept std::initializer_list but not other containers
// so I have to write a generic one here
namespace {
  template <typename C, typename T>
  class SampleGenerator {
  public:

    SampleGenerator(C container):
      _container(std::vector<typename C::value_type>(std::begin(container), std::end(container))) {}

    T unGen(cppqc::RngEngine &rng, std::size_t) {
      return sample(_container, rng);
    }

    std::vector<T> shrink(const T&) {return std::vector<T>();}

  private:
    std::vector<typename C::value_type> _container;

    template <typename S>
    typename std::enable_if<std::is_same<typename S::value_type, T>::value, T>::type
    sample(S container, cppqc::RngEngine &rng) {
      return random_take(container, rng);
    }

    template <typename S>
    typename std::enable_if<!std::is_same<typename S::value_type, T>::value, T>::type
    sample(S container, cppqc::RngEngine &rng) {
      return sample(random_take(container, rng), rng);
    }

  }; /* SampleGenerator */
}

// take one element from (multi-dimension) container
template<typename C, typename T>
SampleGenerator<C, T> sample_of(C container) {
  return std::move(SampleGenerator<C, T>(container));
}

#endif /* GTEST_QUICKCHECK_COORPERATE_YANG_LIU_021 */
