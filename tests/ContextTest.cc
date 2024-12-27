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

  auto &module = ctx.CreateModule("mymodule");
  module.SetProperty("int_value", 5);
  module.SetLambdaProperty<void(uint32_t)>(
      "print", [](uint32_t age) { printf("lambda:%d\n", age); });

  EXPECT_EQ(module.Property("int_value").As<int32_t>(), 5);

  const std::string s = R"xxx(
     import * as my from 'mymodule';

     globalThis.jsprint = function(age, name){
       console.log(age, name);
       return age+1000
     }
     jsprint(100)
    
     console.log(my.int_value);
     globalThis.gint = 20
     print(globalThis.gint)

     my.print(12345)
  )xxx";
  try {
    auto ret = ctx.Eval(s, false);
    EXPECT_EQ(ctx.Global().Property("gint").As<int32_t>(), 20);

    const auto var =
        ctx.Global().Property("jsprint")(300, "name").As<int32_t>();
    printf("var:%d\n", var);

  } catch (const qjs::Exception &e) {
    puts(e.String().c_str());
  }
}
