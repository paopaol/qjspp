#include "qjs++/QJS++.h"
#include "qjs++/impl/traits/FloatValueTraits.h"
#include "qjs++/impl/traits/FunctionValueTraits.h"
#include "qjs++/impl/traits/IntegerValueTraits.h"
#include "qjs++/impl/traits/StringValueTraits.h"
#include "qjs++/impl/traits/VectorValueTraits.h"
#include "gtest/gtest.h"

namespace qjs {

class MyClass {
public:
  MyClass(const std::string &name, int32_t age);

  void Method(int32_t age, const std::string &name) {}

  void ConstMethod(int32_t age, const std::string &name) const {}

  int MethodInt(int32_t age, const std::string &name) { return 1; }

  int ConstMethodInt(int32_t age, const std::string &name) const { return 1; }

  static std::string StaticMethod(int32_t, const std::string &name) {
    return "";
  }
};

class ValueTraitsTest : public testing::Test {
public:
  ValueTraitsTest() : ctx(rt) {}

  qjs::Runtime rt;
  qjs::Context ctx;
};

TEST_F(ValueTraitsTest, JSValue) {
  JSValue v = ValueTraits<JSValue>::Wrap(ctx.Get(), JS_NULL);
}

TEST_F(ValueTraitsTest, Float) {
  JSValue v = ValueTraits<float>::Wrap(ctx.Get(), 3.4);
  EXPECT_FLOAT_EQ(3.4, ValueTraits<float>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, Double) {
  JSValue v = ValueTraits<double>::Wrap(ctx.Get(), 3.4);
  EXPECT_FLOAT_EQ(3.4, ValueTraits<double>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, int16) {
  JSValue v = ValueTraits<int16_t>::Wrap(ctx.Get(), -5);
  EXPECT_EQ(-5, ValueTraits<int16_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, uint16) {
  JSValue v = ValueTraits<uint16_t>::Wrap(ctx.Get(), 10);
  EXPECT_EQ(10, ValueTraits<uint16_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, int32) {
  JSValue v = ValueTraits<int32_t>::Wrap(ctx.Get(), -10);
  EXPECT_EQ(-10, ValueTraits<int32_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, uint32) {
  JSValue v = ValueTraits<uint32_t>::Wrap(ctx.Get(), 100);
  EXPECT_EQ(100, ValueTraits<uint32_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, int64) {
  JSValue v = ValueTraits<int64_t>::Wrap(ctx.Get(), -1000);
  EXPECT_EQ(-1000, ValueTraits<int64_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, uint64) {
  JSValue v = ValueTraits<uint64_t>::Wrap(ctx.Get(), 1000);
  EXPECT_EQ(1000, ValueTraits<uint64_t>::Unwrap(ctx.Get(), v));
}

TEST_F(ValueTraitsTest, Bool) {
  JSValue v = ValueTraits<bool>::Wrap(ctx.Get(), true);
  EXPECT_TRUE(ValueTraits<bool>::Unwrap(ctx.Get(), v));
  JS_FreeValue(ctx.Get(), v);
}

TEST_F(ValueTraitsTest, String) {
  JSValue v = ValueTraits<std::string>::Wrap(ctx.Get(), "string");
  EXPECT_EQ("string", ValueTraits<std::string>::Unwrap(ctx.Get(), v));
  JS_FreeValue(ctx.Get(), v);
}

static int64_t c_funciton(int32_t i32, int64_t i64) { return i32 + i64; }

TEST_F(ValueTraitsTest, CFunction) {
  JSValue v = ValueTraits<QJSFunction<int64_t (*)(int32_t, int64_t)>>::Wrap(
      ctx.Get(), c_funciton);
  EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
  JS_FreeValue(ctx.Get(), v);
}

TEST_F(ValueTraitsTest, Functional) {
  JSValue v = ValueTraits<QJSFunction<std::function<int(int)>>>::Wrap(
      ctx.Get(), [](int) { return 0; });
  EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
  JS_FreeValue(ctx.Get(), v);
}

TEST_F(ValueTraitsTest, ClassMemberMethod) {

  {
    JSValue v = ValueTraits<QJSFunction<void (MyClass::*)(
        int32_t, const std::string &)>>::Wrap(ctx.Get(), &MyClass::Method);
    EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
    JS_FreeValue(ctx.Get(), v);
  }
  {
    JSValue v = ValueTraits<QJSFunction<void (MyClass::*)(
        int32_t, const std::string &) const>>::Wrap(ctx.Get(),
                                                    &MyClass::ConstMethod);
    EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
    JS_FreeValue(ctx.Get(), v);
  }

  {
    JSValue v = ValueTraits<QJSFunction<int (MyClass::*)(
        int32_t, const std::string &)>>::Wrap(ctx.Get(), &MyClass::MethodInt);
    EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
    JS_FreeValue(ctx.Get(), v);
  }

  {
    JSValue v = ValueTraits<QJSFunction<int (MyClass::*)(
        int32_t, const std::string &) const>>::Wrap(ctx.Get(),
                                                    &MyClass::ConstMethodInt);
    EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
    JS_FreeValue(ctx.Get(), v);
  }

  {
    JSValue v = ValueTraits<QJSFunction<std::string (*)(
        int32_t, const std::string &)>>::Wrap(ctx.Get(), MyClass::StaticMethod);
    EXPECT_TRUE(JS_IsFunction(ctx.Get(), v));
    JS_FreeValue(ctx.Get(), v);
  }
}

TEST_F(ValueTraitsTest, Vector) {
  {
    using ArrayType = std::vector<uint32_t>;
    ArrayType vec{1, 2, 3};

    JSValue v = ValueTraits<ArrayType>::Wrap(ctx.Get(), vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx.Get(), v),
              ArrayType({1, 2, 3}));
    JS_FreeValue(ctx.Get(), v);
  }
  {
    using ArrayType = std::vector<std::string>;
    ArrayType vec{"a", "b"};

    JSValue v = ValueTraits<ArrayType>::Wrap(ctx.Get(), vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx.Get(), v),
              ArrayType({"a", "b"}));
    JS_FreeValue(ctx.Get(), v);
  }
  {
    using ArrayType = std::vector<float>;

    ArrayType vec{3, 5, 8};
    JSValue v = ValueTraits<ArrayType>::Wrap(ctx.Get(), vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx.Get(), v),
              ArrayType({3, 5, 8}));
    JS_FreeValue(ctx.Get(), v);
  }
}

TEST_F(ValueTraitsTest, Array) {
  {
    using ArrayType2 = std::array<uint32_t, 2>;
    using ArrayType3 = std::array<uint32_t, 3>;
    ArrayType3 vec{1, 2, 3};

    JSValue v = ValueTraits<ArrayType3>::Wrap(ctx.Get(), vec);
    EXPECT_EQ(ValueTraits<ArrayType3>::Unwrap(ctx.Get(), v),
              ArrayType3({1, 2, 3}));
    EXPECT_EQ(ValueTraits<ArrayType2>::Unwrap(ctx.Get(), v),
              ArrayType2({1, 2}));
    JS_FreeValue(ctx.Get(), v);
  }
}

TEST_F(ValueTraitsTest, Queue) {
  {
    using Queue = std::deque<uint32_t>;

    Queue q;

    q.push_back(1);
    q.push_back(2);

    JSValue v = ValueTraits<Queue>::Wrap(ctx.Get(), q);
    EXPECT_EQ(ValueTraits<Queue>::Unwrap(ctx.Get(), v), q);
    JS_FreeValue(ctx.Get(), v);
  }
}

TEST_F(ValueTraitsTest, SharedPointer) {
  auto v = std::make_shared<int32_t>(123);
  JSValue jv = ValueTraits<std::shared_ptr<int32_t>>::Wrap(ctx.Get(), v);
  auto cv = ValueTraits<std::shared_ptr<int32_t>>::Unwrap(ctx.Get(), jv);

  EXPECT_EQ(123, *cv);
  JS_FreeValue(ctx.Get(), jv);
}

TEST_F(ValueTraitsTest, UniquePointer) {
  auto v = std::make_unique<int32_t>(123);
  JSValue jv = ValueTraits<std::unique_ptr<int32_t>>::Wrap(ctx.Get(), v);
  auto cv = ValueTraits<std::unique_ptr<int32_t>>::Unwrap(ctx.Get(), jv);

  EXPECT_EQ(123, *cv);
  JS_FreeValue(ctx.Get(), jv);
}
} // namespace qjs
