#pragma once

#include <cstring>
#include <string>

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"

namespace qjs {

template <> struct ValueTraits<std::string> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    const char *ptr = JS_ToCString(ctx, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, const std::string &v) {
    return JS_NewStringLen(ctx, v.data(), v.size());
  }
};

template <> struct ValueTraits<const char *> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr, len);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, const char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <> struct ValueTraits<char *> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr, len);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <std::size_t N> struct ValueTraits<char (&)[N]> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr, len);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <std::size_t N> struct ValueTraits<const char (&)[N]> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException(ctx);

    std::string s(ptr, len);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, const char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};
} // namespace qjs
