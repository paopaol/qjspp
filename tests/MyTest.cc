#include "qjs++/qjs++ex.h"
#include "gtest/gtest.h"

struct Point {
  int32_t x = 0;
  int32_t y = 0;

  Point(int32_t x, int32_t y) : x(x), y(y) {}

  int Dump(const std::string &prefix) { return 0; }
};

static int32_t Print1(int a) {
  printf("value:%d\n", a);
  return a * 2;
}

static int32_t Print2(int a, int b) { return a + 1; }

TEST(property, Function) {
  QJSRuntime runtime;
  QJSContext ctx(runtime);

  auto global = ctx.Global();

  global["Print"] = Print1;
  global["Print2"] = Print2;
  global["Print3"].Function<int32_t(int, int)>(
      [](int a, int b) { return Print2(a, b); });

  global["Print"](100);
  global["Print2"](200, 3000);
  global["Print3"](300);

  const char *script = R"(
     Print3(500, 4000);
   )";

  Print2(500, 4000);
  ctx.Eval(script);
}

TEST(Context, Module) {
  QJSRuntime runtime;
  QJSContext ctx(runtime);

  ctx.Module("my_module")["Print"].Function<int(int)>(Print1);

  ctx.Eval(R"xxx(
            import * as my from 'my_module';
            var ret = my.Print(333);
            return ret;
        )xxx");

  auto result = ctx.Module("my_module")["Print"](123).As<uint32_t>();
  printf("result %d\n", result);
}

// TEST(Class, Registor) {
//   QJSRuntime runtime;
//   QJSContext ctx(runtime);
//
//   QJSClass<Point>::RegisterClass(nullptr, "Point");
//
//   QJSValueTraits<QJSClassHelper<Point>>::Wrap<int32_t, int32_t>(
//       nullptr, QJSClassHelper<Point>("Point"));
// }
