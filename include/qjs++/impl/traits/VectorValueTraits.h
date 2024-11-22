#pragma once

#include "qjs++/impl/Exception.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"
#include <deque>
#include <vector>

template <typename T> struct ArrayHelper {
  static void Push(JSContext *ctx, JSValue array, uint32_t index, T v) {
    JS_SetPropertyUint32(ctx, array, index, ValueTraits<T>::Wrap(ctx, v));
  }

  template <typename Array> static JSValue New(JSContext *ctx, const Array &v) {
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

template <typename T,
          template <typename U, typename = std::allocator<U>> class Container>
struct ValueTraits<
    Container<T, std::allocator<T>>,
    typename std::enable_if<
        std::is_same<Container<T>, std::vector<T, std::allocator<T>>>::value ||
        std::is_same<Container<T>,
                     std::deque<T, std::allocator<T>>>::value>::type> {
  static Container<T> Unwrap(JSContext *ctx, JSValueConst v) {
    Container<T> vec;

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

  static JSValue Wrap(JSContext *ctx, const Container<T> &v) {
    return ArrayHelper<T>::New(ctx, v);
  }
};

template <typename T, std::size_t N> struct ValueTraits<std::array<T, N>> {
  static std::array<T, N> Unwrap(JSContext *ctx, JSValueConst v) {
    std::array<T, N> vec;

    JSValue length = JS_GetPropertyStr(ctx, v, "length");
    if (JS_IsException(length)) {
      throw QJSException(ctx);
    }

    int len = JS_VALUE_GET_INT(length);
    for (int i = 0; i < len && i < N; i++) {
      vec[i] = std::move(ArrayHelper<T>::At(ctx, v, i));
    }

    return vec;
  }

  static JSValue Wrap(JSContext *ctx, const std::array<T, N> &v) {
    return ArrayHelper<T>::New(ctx, v);
  }
};
