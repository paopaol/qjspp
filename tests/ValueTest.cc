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

TEST_F(ValueTest, ConstructFrom) {
  qjs::Value v(&ctx, 123);
  EXPECT_EQ(v.As<int32_t>(), 123);
}

TEST_F(ValueTest, CopyConstruct) {}

TEST_F(ValueTest, MoveConstruct) {
  qjs::Value a(&ctx, 123);

  qjs::Value b(std::move(a));

  EXPECT_EQ(b.As<uint32_t>(), 123);
  EXPECT_TRUE(a.IsUndefined());
}

TEST_F(ValueTest, CopyAssgin) {}

TEST_F(ValueTest, MoveAssgin) {
  qjs::Value b;
  qjs::Value a(&ctx, 123);

  b = std::move(a);

  EXPECT_EQ(b.As<uint32_t>(), 123);
  EXPECT_TRUE(a.IsUndefined());
}

TEST_F(ValueTest, AssginInt) {
  qjs::Value v(&ctx);

  v = 123;
  EXPECT_EQ(v.As<int32_t>(), 123);
  v = std::string("123");
  v = "123";

  char s[12] = "123";
  v = s;
}

TEST_F(ValueTest, AssginString) {
  qjs::Value v(&ctx);

  v = std::string("123");
  EXPECT_EQ(v.As<std::string>(), "123");

  v = "456";
  EXPECT_EQ(v.As<std::string>(), "456");

  char s[12] = "666";
  v = s;
  EXPECT_EQ(v.As<std::string>(), "666");
}

TEST_F(ValueTest, AssginFloating) {
  qjs::Value v(&ctx);

  v = 3.f;
  EXPECT_FLOAT_EQ(v.As<float>(), 3.f);
  EXPECT_FLOAT_EQ(v.As<double>(), 3.f);
}

TEST_F(ValueTest, AssginVector) {
  qjs::Value v(&ctx);

  v = std::vector<int64_t>{1, 2, 3};
  EXPECT_EQ(v.As<std::vector<int64_t>>(), std::vector<int64_t>({1, 2, 3}));
}
