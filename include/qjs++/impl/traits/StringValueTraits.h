#pragma once

#include <cstring>
#include <string>

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs.h"

namespace qjs {

template <> struct ValueTraits<std::string> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    const char *ptr = JS_ToCString(ctx, v);
    if (!ptr)
      throw Exception(ctx);

    std::string s(ptr);
    JS_FreeCString(ctx, ptr);
    return s;
  }

  static JSValue Wrap(JSContext *ctx, const std::string &v) {
    return JS_NewStringLen(ctx, v.data(), v.size());
  }
};

struct JSString {
  JSContext *ctx = nullptr;
  const char *data = nullptr;
  std::size_t len = 0;

  JSString(JSContext *ctx, const char *ptr, std::size_t len_)
      : ctx(ctx), data(ptr), len(len_) {}

  JSString(const JSString &other) = delete;

  JSString &operator=(const JSString &other) = delete;

  JSString(JSString &&other) {
    Free();
    ctx = other.ctx;
    data = other.data;
    len = other.len;

    other.ctx = nullptr;
    other.data = nullptr;
    other.len = 0;
  }

  operator const char *() const { return data; }

  ~JSString() { Free(); }

private:
  void Free() {
    if (ctx) {
      JS_FreeCString(ctx, data);
    }
  }
};

template <> struct ValueTraits<const char *> {
  static JSString Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr) {
      throw Exception(ctx);
    }
    return JSString(ctx, ptr, len);
  }

  static JSValue Wrap(JSContext *ctx, const char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <> struct ValueTraits<char *> {
  static JSString Unwrap(JSContext *ctx, JSValueConst v) {
    return ValueTraits<const char *>::Unwrap(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <std::size_t N> struct ValueTraits<char (&)[N]> {
  static JSString Unwrap(JSContext *ctx, JSValueConst v) {
    return ValueTraits<const char *>::Unwrap(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};

template <std::size_t N> struct ValueTraits<const char (&)[N]> {
  static JSString Unwrap(JSContext *ctx, JSValueConst v) {
    return ValueTraits<const char *>::Unwrap(ctx, v);
  }

  static JSValue Wrap(JSContext *ctx, const char *v) {
    return JS_NewStringLen(ctx, v, std::strlen(v));
  }
};
} // namespace qjs
