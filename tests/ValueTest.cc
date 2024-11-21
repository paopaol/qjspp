#include "qjs++/QJS++.h"

#include "gtest/gtest.h"

class ValueTest : public testing::Test {
public:
  ValueTest() : ctx(rt) {}

  qjs::Runtime rt;
  qjs::Context ctx;
};

TEST_F(ValueTest, Construct) {
  qjs::Value v;
  EXPECT_TRUE(v.IsUndefined());
}

TEST_F(ValueTest, ConstructFromInt) {
  qjs::Value v(&ctx, 123);
  EXPECT_EQ(v.As<int32_t>(), 123);
}
