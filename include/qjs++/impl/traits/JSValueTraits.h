#pragma once

#include "quickjs/quickjs.h"

template <typename T, typename = void> struct JSValueTraits {
  static T Unwrap(JSContext *ctx, JSValueConst v) = delete;

  static JSValue Wrap(JSContext *ctx, T v) = delete;
};

template <> struct JSValueTraits<JSValue> {
  static JSValue Unwrap(JSContext *ctx, JSValueConst v) {
    return JS_DupValue(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, const JSValue &v) {
    return JS_DupValue(ctx, v);
  }
};
