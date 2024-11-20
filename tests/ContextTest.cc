#include "qjs++/QJS++.h"

#include "gtest/gtest.h"

TEST(Context, construct) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);
}
