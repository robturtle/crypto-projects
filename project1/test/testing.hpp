#ifndef GTEST_QUICKCHECK_COORPERATE_YANG_LIU_0211
#define GTEST_QUICKCHECK_COORPERATE_YANG_LIU_0211
#include "gtest/gtest.h"
#include "cppqc.h"
#include <vector>

// specify how a new data type varies and shrinks
// register it so the it can play as the type variable in QuickCheck
#define REGISTER_ARBITRARY(Class, gen, shrinker)      \
  template <> struct cppqc::ArbitraryImpl<Class> { \
    static const typename cppqc::Arbitrary<Class>::unGenType unGen;  \
    static const typename cppqc::Arbitrary<Class>::shrinkType shrink;  \
  };\
  const typename cppqc::Arbitrary<Class>::unGenType \
  cppqc::ArbitraryImpl<Class>::unGen = [](cppqc::RngEngine &rng, std::size_t size) { \
    return gen(rng, size);\
  }; \
  const typename cppqc::Arbitrary<Class>::shrinkType \
  cppqc::ArbitraryImpl<Class>::shrink = [](Class c) { \
    return shrinker(c);\
  }


// wrap a quickcheck runner into a gtest test case
#define RUN_QUICK_CHECK(fixture, property) TEST (fixture, property)    \
  {EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(property()).result);}


#define RUN_QUICK_CHECK_F(fixture, property) TEST_F (fixture, property)    \
  {EXPECT_EQ(cppqc::QC_SUCCESS, cppqc::quickCheckOutput(property()).result);}

#endif /* GTEST_QUICKCHECK_COORPERATE_YANG_LIU_021 */
