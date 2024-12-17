#include "qjs++/QJS++.h"

#include "gtest/gtest.h"

TEST(Context, construct) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);
}

TEST(Context, NewObject) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  qjs::Value v = ctx.NewObject();
  EXPECT_TRUE(v.IsObject());
  v.SetProperty("int", 123);

  EXPECT_EQ(v["int"].As<int32_t>(), 123);
}

static void print(int a) {
  a = a + 1;
  printf("%d\n", a);
}

TEST(Context, CreateModule) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  ctx.Global().SetProperty("print", print);
  ctx.Global().SetProperty("gint", 10);

  auto &module = ctx.CreateModule("math");
  module.SetProperty("int_value", 5);

  EXPECT_EQ(module.Property("int_value").As<int32_t>(), 5);

  const std::string s = R"xxx(
    import * as my from 'math';

    print(my.int_value);
    globalThis.gint = 20
    print(globalThis.gint)
  )xxx";
  try {
    auto ret = ctx.Eval(s);
    EXPECT_EQ(ctx.Global().Property("gint").As<int32_t>(), 20);
  } catch (const qjs::Exception &e) {
    puts(e.String().c_str());
  }
}
