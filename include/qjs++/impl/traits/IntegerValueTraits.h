#pragma once

#include "JSValueTraits.h"
#include "qjs++/impl/Exception.h"
#include "quickjs/quickjs.h"
#include <type_traits>

template <typename T>
static constexpr auto IsSmallUnsignedInteger =
    std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value;

template <typename T>
static constexpr auto IsSmallSignedInteger =
    std::is_same<T, int32_t>::value || std::is_same<T, int16_t>::value;

template <typename T>
static constexpr auto Is64Integer =
    std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value;

template <typename T>
struct JSValueTraits<T,
                     typename std::enable_if<IsSmallSignedInteger<T>>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    int32_t t;

    if (JS_ToInt32(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewInt32(ctx, v); }
};

template <typename T>
struct JSValueTraits<T,
                     typename std::enable_if<IsSmallUnsignedInteger<T>>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    uint32_t t;

    if (JS_ToUint32(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewUint32(ctx, v); }
};

template <typename T>
struct JSValueTraits<T, typename std::enable_if<Is64Integer<T>>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    int64_t t;

    if (JS_ToInt64(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewInt64(ctx, v); }
};

template <> struct JSValueTraits<bool> {
  static bool Unwrap(JSContext *ctx, JSValueConst v) {
    auto b = JS_ToBool(ctx, v);
    if (b < 0) {
      throw QJSException(ctx);
    }
    return b;
  }

  static JSValue Wrap(JSContext *ctx, bool v) { return JS_NewBool(ctx, v); }
};
