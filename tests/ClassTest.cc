#include "qjs++/QJS++.h"

#include "gtest/gtest.h"

class Point {
public:
  Point(float x, float y) : x_(x), y_(y) {
    printf("Point Constructor called x:%f y:%f\n", x_, y_);
  }

  ~Point() {
    printf("~Point Constructor called x:%f y:%f %s\n", x_, y_, name_.c_str());
  }

  float Norm() {
    puts("call Norm Method");

    return std::sqrt(x_ * x_ + y_ * y_);
  }

private:
  float x_ = 0;
  float y_ = 0;
  std::string name_ = "name";
};

namespace qjs {
class ClassTest : public testing::Test {
public:
  ClassTest() {
    int a = 0;
    ctx = std::make_shared<qjs::Context>(rt);
  }

  qjs::Runtime rt;
  std::shared_ptr<qjs::Context> ctx;
};

//TEST_F(ClassTest, ConstructorValueTRaits) {
//  //  QJSCtor<Point *(float, float)> ctor{"Point"};

//  //  Value v(&ctx,
//  //          ValueTraits<QJSCtor<Point *(float, float)>>::Wrap(ctx.Get(),
//  //          ctor));
//  //  EXPECT_TRUE(v.IsFunction());
//}

TEST_F(ClassTest, CreateClass) {
  auto &module = ctx->CreateModule("mymodule");

  module.CreateClass<Point>("Point").Construct<float, float>("Point").Method(
      "Norm", &Point::Norm);

  const std::string s = R"xxx(
         import * as my from 'mymodule';

         var point = new my.Point(3,3)
//       console.log(point.Norm());
    )xxx";
  try {
    auto ret = ctx->Eval(s, false);
  } catch (const qjs::Exception &e) {
    puts(e.String().c_str());
  }
}
} // namespace qjs
