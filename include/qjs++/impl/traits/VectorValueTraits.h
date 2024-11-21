#pragma once

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"
#include <vector>

template <typename T> struct ArrayHelper {
  static void Push(JSContext *ctx, JSValue array, uint32_t index, T v) {
    JS_SetPropertyUint32(ctx, array, index, ValueTraits<T>::Wrap(ctx, v));
  }

  static T At(JSContext *ctx, JSValue array, uint32_t index) {
    auto e = JS_GetPropertyUint32(ctx, array, index);
    if (JS_IsException(e)) {
      throw QJSException(ctx);
    }
    auto v = ValueTraits<T>::Unwrap(ctx, e);
    /**
     * @brief FIXME(jinzhao):auto free
     */
    JS_FreeValue(ctx, e);
    return std::move(v);
  }
};

template <typename T> struct ValueTraits<std::vector<T>> {
  static std::vector<T> Unwrap(JSContext *ctx, JSValueConst v) {
    std::vector<T> vec;

    JSValue length = JS_GetPropertyStr(ctx, v, "length");
    if (JS_IsException(length)) {
      throw QJSException(ctx);
    }

    int len = JS_VALUE_GET_INT(length);
    for (int i = 0; i < len; i++) {
      vec.push_back(std::move(ArrayHelper<T>::At(ctx, v, i)));
    }

    return vec;
  }

  static JSValue Wrap(JSContext *ctx, std::vector<T> v) {
    JSValue array = JS_NewArray(ctx);

    if (JS_IsException(array)) {
      throw QJSException(ctx);
    }

    int i = 0;
    for (const auto &e : v) {
      ArrayHelper<T>::Push(ctx, array, i++, e);
    }

    return array;
  }
};
