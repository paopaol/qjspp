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
