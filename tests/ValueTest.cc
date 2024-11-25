#include "qjs++/QJS++.h"

#include "gtest/gtest.h"

static int32_t c_fun(int32_t v) { return v + 1; }

static int32_t c_string_fun(const char *v) { return std::strlen(v); }

static void void_c_fun(int32_t v) {}

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

TEST_F(ValueTest, IsNull) {
  qjs::Value v(&ctx, JS_NULL);
  EXPECT_TRUE(v.IsNull());
}

TEST_F(ValueTest, IsUndefined) {
  qjs::Value v(&ctx, JS_UNDEFINED);

  EXPECT_TRUE(v.IsUndefined());
}

TEST_F(ValueTest, IsException) {
  qjs::Value v(&ctx, JS_EXCEPTION);
  EXPECT_TRUE(v.IsException());
}

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

TEST_F(ValueTest, CallCFunction) {
  qjs::Value v(&ctx);

  v.SetLambda<int(int)>(c_fun);
  EXPECT_EQ(v(5).As<int32_t>(), 6);

  v = c_fun;
  EXPECT_EQ(v(5).As<int32_t>(), 6);
}

TEST_F(ValueTest, CallStaticClassMethod) {
  class Type {
  public:
    static uint32_t f(uint32_t v) { return v + 1; }
  };

  qjs::Value v(&ctx);

  v = Type::f;
  EXPECT_EQ(v(5).As<int32_t>(), 6);
}

TEST_F(ValueTest, CallVoidLambdaFunction) {
  qjs::Value v(&ctx);

  int32_t var = 0;
  v.SetLambda<void(int)>([&](int a) { var = a + 1; });
  v(5);
  EXPECT_EQ(var, 6);
}

TEST_F(ValueTest, CallVoidCFunction) {
  qjs::Value v(&ctx);

  v = void_c_fun;

  int32_t out = 0;
  v(5);
}

TEST_F(ValueTest, SetProperty) {
  ctx.Global()
      .SetProperty("int", 123)
      .SetProperty("float", 3.f)
      .SetProperty("string", "name")
      .SetProperty("list", std::vector<std::string>({"1", "2", "3"}))
      .SetProperty("c_function", c_fun)
      .SetProperty("labmda",
                   std::function<int32_t(int)>([](int v) { return v + 1; }));

  EXPECT_EQ(ctx.Global()["int"].As<int64_t>(), 123);
  EXPECT_FLOAT_EQ(ctx.Global()["float"].As<float>(), 3.f);
  EXPECT_EQ(ctx.Global()["string"].As<std::string>(), "name");
  EXPECT_EQ(ctx.Global()["list"].As<std::vector<std::string>>(),
            std::vector<std::string>({"1", "2", "3"}));
  EXPECT_EQ(ctx.Global()["c_function"](5).As<int32_t>(), 6);
  EXPECT_EQ(ctx.Global()["labmda"](5).As<int32_t>(), 6);
}

TEST_F(ValueTest, AssignProperty) {
  auto v = ctx.NewObject();

  v["int"] = 123;
  EXPECT_TRUE(v["int"].IsNumber());

  v["int"] = "123";
  EXPECT_TRUE(v["int"].IsString());

  v["float"] = 3.f;
  v["int"] = 123;
  v["string"] = "name";
  v["list"] = std::vector<std::string>({"1", "2", "3"});
  v["c_function"] = c_fun;
  v["labmda"] = std::function<int32_t(int)>([](int v) { return v + 1; });

  EXPECT_EQ(v["int"].As<int64_t>(), 123);
  EXPECT_EQ(v["int"].As<std::string>(), "123");
  EXPECT_FLOAT_EQ(v["float"].As<float>(), 3.f);
  EXPECT_EQ(v["string"].As<std::string>(), "name");
  EXPECT_EQ(v["list"].As<std::vector<std::string>>(),
            std::vector<std::string>({"1", "2", "3"}));
  EXPECT_EQ(v["c_function"](5).As<int32_t>(), 6);
  EXPECT_EQ(v["labmda"](5).As<int32_t>(), 6);
}

TEST_F(ValueTest, BindCNativeFunction) {
  qjs::Value v(&ctx);

  v = puts;
  v(std::string("name").c_str());

  v = strlen;
  EXPECT_EQ(v("abc").As<int32_t>(), 3);
}
