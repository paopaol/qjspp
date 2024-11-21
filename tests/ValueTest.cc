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

TEST_F(ValueTest, IsNull) { qjs::Value v(&ctx); }

TEST_F(ValueTest, IsUndefined) { qjs::Value v(&ctx); }

TEST_F(ValueTest, IsException) { qjs::Value v(&ctx); }

TEST_F(ValueTest, IsBool) {
  qjs::Value v(&ctx);

  v = true;
  EXPECT_TRUE(v.IsBool());
}

TEST_F(ValueTest, IsError) {
  qjs::Value v(&ctx);

  // v = true;
  // EXPECT_TRUE(v.IsBool());
}

TEST_F(ValueTest, IsArray) {
  qjs::Value v(&ctx);

  v = std::vector<std::string>{"1", "b"};
  EXPECT_TRUE(v.IsArray());
}

TEST_F(ValueTest, IsNumber) {
  qjs::Value v(&ctx);

  v = 345;
  EXPECT_TRUE(v.IsNumber());
}

TEST_F(ValueTest, IsString) {
  qjs::Value v(&ctx);

  v = "";
  EXPECT_TRUE(v.IsString());
}

TEST_F(ValueTest, IsBigInt) {
  qjs::Value v(&ctx);

  // v = 999;
  // EXPECT_TRUE(v.IsBigInt());
}

TEST_F(ValueTest, IsObject) {
  qjs::Value v(&ctx);

  // v = true;
  // EXPECT_TRUE(v.IsBool());
}

TEST_F(ValueTest, IsSymbol) {
  qjs::Value v(&ctx);

  // v = true;
  // EXPECT_TRUE(v.IsBool());
}

TEST_F(ValueTest, IsBigFloat) {
  qjs::Value v(&ctx);

  // v = true;
  // EXPECT_TRUE(v.IsBool());
}

static int32_t c_fun() { return 0; }

TEST_F(ValueTest, IsFunction) {
  qjs::Value v(&ctx);

  v = c_fun;
  EXPECT_TRUE(v.IsFunction());

  v.SetLambda<int(int)>([](int) { return 0; });
  EXPECT_TRUE(v.IsFunction());
}

TEST_F(ValueTest, CallLambda) {
  qjs::Value v(&ctx);

  v.SetLambda<int(int)>([](int var) { return var + 1; });
  EXPECT_EQ(v(5).As<int32_t>(), 6);
}
