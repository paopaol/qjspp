#pragma once

#include "qjs++/impl/Exception.h"
#include "quickjs.h"
#include <memory>

namespace qjs {

template <typename T> struct QJSFunction;

template <typename T> struct QJSCtor;

template <typename T, typename = void> struct ValueTraits {
  static T Unwrap(JSContext *ctx, JSValueConst v) = delete;

  static JSValue Wrap(JSContext *ctx, T v) = delete;
};

template <> struct ValueTraits<JSValue> {
  static JSValue Unwrap(JSContext *ctx, JSValueConst v) {
    return JS_DupValue(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, const JSValue v) {
    return JS_DupValue(ctx, v);
  }
};

template <> struct ValueTraits<void> {
  static JSValue Unwrap(JSContext *ctx, JSValueConst v) {
    throw Exception(ctx);
  }
};

template <typename T> struct ValueTraits<std::shared_ptr<T>> {
  static std::shared_ptr<T> Unwrap(JSContext *ctx, JSValueConst v) {
    if (JS_IsNull(v) || JS_IsUndefined(v)) {
      return nullptr;
    }

    return std::make_shared<T>(ValueTraits<T>::Unwrap(ctx, v));
  }

  static JSValue Wrap(JSContext *ctx, const std::shared_ptr<T> &v) {
    return v ? ValueTraits<T>::Wrap(ctx, *v) : JS_NULL;
  }
};

template <typename T> struct ValueTraits<std::unique_ptr<T>> {
  static std::unique_ptr<T> Unwrap(JSContext *ctx, JSValueConst v) {
    if (JS_IsNull(v) || JS_IsUndefined(v)) {
      return nullptr;
    }

    return std::unique_ptr<T>(new T(ValueTraits<T>::Unwrap(ctx, v)));
  }

  static JSValue Wrap(JSContext *ctx, const std::unique_ptr<T> &v) {
    return v ? ValueTraits<T>::Wrap(ctx, *v) : JS_NULL;
  }
};
} // namespace qjs
