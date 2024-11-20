#pragma once

#include <string>

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"

template <> struct JSValueTraits<std::string> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr, len);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, const std::string &v) {
    return JS_NewStringLen(ctx, v.data(), v.size());
  }
};
