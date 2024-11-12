#pragma once

#include <string>
#include <type_traits>

#include "QJSException.h"
#include "qjs++/private/QJSWrapper.h"
#include "quickjs/quickjs.h"

template <typename T, typename = void>
struct QJSValueTraits {
  /**
   * @brief 将JSValueConst转换成c++内置类型
   */
  static T Unwrap(JSContext *ctx, JSValueConst v) = delete;

  /**
   * @brief 将c++内置类型转换成JSValue
   */
  static JSValue Wrap(JSContext *ctx, T v) = delete;
};

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_same<int32_t, T>::value ||
                               std::is_same<int16_t, T>::value ||
                               std::is_same<uint16_t, T>::value>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    int32_t t;

    if (JS_ToInt32(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewInt32(ctx, v); }
};

template <>
struct QJSValueTraits<uint32_t> {
  static uint32_t Unwrap(JSContext *ctx, JSValueConst v) {
    uint32_t t;

    if (JS_ToUint32(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, uint32_t v) {
    return JS_NewInt32(ctx, v);
  }
};

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_same<int64_t, T>::value ||
                               std::is_same<uint64_t, T>::value>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    int64_t t;

    if (JS_ToInt64(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewInt64(ctx, v); }
};

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
  static T Unwrap(JSContext *ctx, JSValueConst v) {
    double t;

    if (JS_ToFloat64(ctx, &t, v) < 0) {
      throw QJSException(ctx);
    }
    return t;
  }

  static JSValue Wrap(JSContext *ctx, T v) { return JS_NewFloat64(ctx, v); }
};

template <>
struct QJSValueTraits<std::string> {
  static std::string Unwrap(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr) throw QJSException(ctx);

    return std::string(ptr, len);
  }

  static JSValue Wrap(JSContext *ctx, const std::string &v) {
    return JS_NewStringLen(ctx, v.data(), v.size());
  }
};

template <typename T>
struct QJSValueTraits<T *,
                      typename std::enable_if<std::is_class<T>::value>::type> {
  static JSValue Wrap(JSContext *ctx, T *v) {
    auto inst = JS_NewObjectClass(ctx, QJSClass<T>::id_);
    if (JS_IsException(inst)) {
      return inst;
    }
    JS_SetOpaque(inst, v);
    return inst;
  }
};

template <>
struct QJSValueTraits<QJSValue> {
  static JSValue Wrap(JSContext *ctx, const QJSValue &v) {
    return JS_DupValue(ctx, v.Raw());
  }
};

template <>
struct QJSValueTraits<JSValue> {
  static JSValue Wrap(JSContext *ctx, const JSValue &v) {
    return JS_DupValue(ctx, v);
  }
};
