#include "qjs++/impl/traits/FloatValueTraits.h"
#include "qjs++/impl/traits/FunctionValueTraits.h"
#include "qjs++/impl/traits/IntegerValueTraits.h"
#include "qjs++/impl/traits/StringValueTraits.h"
#include "qjs++/impl/traits/VectorValueTraits.h"
#include "gtest/gtest.h"

class ValueTraitsTest : public testing::Test {
public:
  void SetUp() override {
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
  }

  ~ValueTraitsTest() {
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
  }

  JSRuntime *rt = nullptr;
  JSContext *ctx = nullptr;
};

TEST_F(ValueTraitsTest, Float) {
  JSValue v = ValueTraits<float>::Wrap(ctx, 3.4);
  EXPECT_FLOAT_EQ(3.4, ValueTraits<float>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, Double) {
  JSValue v = ValueTraits<double>::Wrap(ctx, 3.4);
  EXPECT_FLOAT_EQ(3.4, ValueTraits<double>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, int16) {
  JSValue v = ValueTraits<int16_t>::Wrap(ctx, -5);
  EXPECT_EQ(-5, ValueTraits<int16_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, uint16) {
  JSValue v = ValueTraits<uint16_t>::Wrap(ctx, 10);
  EXPECT_EQ(10, ValueTraits<uint16_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, int32) {
  JSValue v = ValueTraits<int32_t>::Wrap(ctx, -10);
  EXPECT_EQ(-10, ValueTraits<int32_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, uint32) {
  JSValue v = ValueTraits<uint32_t>::Wrap(ctx, 100);
  EXPECT_EQ(100, ValueTraits<uint32_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, int64) {
  JSValue v = ValueTraits<int64_t>::Wrap(ctx, -1000);
  EXPECT_EQ(-1000, ValueTraits<int64_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, uint64) {
  JSValue v = ValueTraits<uint64_t>::Wrap(ctx, 1000);
  EXPECT_EQ(1000, ValueTraits<uint64_t>::Unwrap(ctx, v));
}

TEST_F(ValueTraitsTest, Bool) {
  JSValue v = ValueTraits<bool>::Wrap(ctx, true);
  EXPECT_TRUE(ValueTraits<bool>::Unwrap(ctx, v));
  JS_FreeValue(ctx, v);
}

TEST_F(ValueTraitsTest, String) {
  JSValue v = ValueTraits<std::string>::Wrap(ctx, "string");
  EXPECT_EQ("string", ValueTraits<std::string>::Unwrap(ctx, v));
  JS_FreeValue(ctx, v);
}

static int64_t c_funciton(int32_t i32, int64_t i64) { return i32 + i64; }

TEST_F(ValueTraitsTest, CFunction) {
  JSValue v = ValueTraits<QJSFunction<int64_t (*)(int32_t, int64_t)>>::Wrap(
      ctx, c_funciton);
  EXPECT_TRUE(JS_IsFunction(ctx, v));
  JS_FreeValue(ctx, v);
}

TEST_F(ValueTraitsTest, Functional) {
  JSValue v = ValueTraits<QJSFunction<std::function<int(int)>>>::Wrap(
      ctx, [](int) { return 0; });
  EXPECT_TRUE(JS_IsFunction(ctx, v));
  JS_FreeValue(ctx, v);
}

TEST_F(ValueTraitsTest, ClassMemberMethod) {
  class Class {
  public:
    void Method(int32_t age, const std::string &name) {}

    void ConstMethod(int32_t age, const std::string &name) const {}

    static std::string StaticMethod(int32_t, const std::string &name) {
      return "";
    }
  };

  {
    JSValue v = ValueTraits<QJSFunction<void (Class::*)(
        int32_t, const std::string &)>>::Wrap(ctx, &Class::Method);
    EXPECT_TRUE(JS_IsFunction(ctx, v));
    JS_FreeValue(ctx, v);
  }
  {
    JSValue v = ValueTraits<QJSFunction<void (Class::*)(
        int32_t, const std::string &) const>>::Wrap(ctx, &Class::ConstMethod);
    EXPECT_TRUE(JS_IsFunction(ctx, v));
    JS_FreeValue(ctx, v);
  }

  {
    JSValue v = ValueTraits<QJSFunction<std::string (*)(
        int32_t, const std::string &)>>::Wrap(ctx, Class::StaticMethod);
    EXPECT_TRUE(JS_IsFunction(ctx, v));
    JS_FreeValue(ctx, v);
  }
}

TEST_F(ValueTraitsTest, Vector) {
  {
    using ArrayType = std::vector<uint32_t>;
    ArrayType vec{1, 2, 3};

    JSValue v = ValueTraits<ArrayType>::Wrap(ctx, vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx, v), ArrayType({1, 2, 3}));
    JS_FreeValue(ctx, v);
  }
  {
    using ArrayType = std::vector<std::string>;
    ArrayType vec{"a", "b"};

    JSValue v = ValueTraits<ArrayType>::Wrap(ctx, vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx, v), ArrayType({"a", "b"}));
    JS_FreeValue(ctx, v);
  }
  {
    using ArrayType = std::vector<float>;

    ArrayType vec{3, 5, 8};
    JSValue v = ValueTraits<ArrayType>::Wrap(ctx, vec);
    EXPECT_EQ(ValueTraits<ArrayType>::Unwrap(ctx, v), ArrayType({3, 5, 8}));
    JS_FreeValue(ctx, v);
  }
}
