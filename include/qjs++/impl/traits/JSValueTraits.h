#pragma once

#include "qjs++/impl/Exception.h"
#include "quickjs/quickjs.h"

template <typename T, typename = void> struct ValueTraits {
  static T Unwrap(JSContext *ctx, JSValueConst v) = delete;

  static JSValue Wrap(JSContext *ctx, T v) = delete;
};

template <> struct ValueTraits<JSValue> {
  static JSValue Unwrap(JSContext *ctx, JSValueConst v) {
    return JS_DupValue(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, const JSValue &v) {
    return JS_DupValue(ctx, v);
  }
};

template <> struct ValueTraits<void> {
  static JSValue Unwrap(JSContext *ctx, JSValueConst v) {
    throw QJSException(ctx);
  }
};

template <typename T> struct QJSFunction;
