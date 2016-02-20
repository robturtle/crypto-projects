#include "gtest/gtest.h"
#include "cppqc.h"

int factorial(int n) {
  int acc = 1;
  for (int i = 1; i <= n; i++) acc *= i;
  return acc;
}

TEST (FactorialTest, ZeroInput) {
  EXPECT_EQ(1, factorial(0));
}

TEST (FactorialTest, PositiveInput) {
  EXPECT_EQ(1, factorial(1));
  EXPECT_EQ(2, factorial(2));
  EXPECT_EQ(6, factorial(3));
  EXPECT_EQ(40320, factorial(8));
}
