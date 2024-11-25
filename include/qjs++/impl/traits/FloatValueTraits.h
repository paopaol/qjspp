#pragma once

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include <type_traits>

namespace qjs {

template <typename T>
static constexpr bool IsFloatPoint = std::is_floating_point<T>::value;

template <typename T>
struct ValueTraits<T, typename std::enable_if<IsFloatPoint<T>>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    double t;

    if (JS_ToFloat64(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewFloat64(ctx, v); }
};
} // namespace qjs
