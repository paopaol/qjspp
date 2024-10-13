#pragma once

#include "absl/utility/utility.h"
#include "quickjs/quickjs.h"
#include <exception>
#include <string>
#include <type_traits>
#include <utility>

class QJSException : public std::exception {};

template <typename T, typename = void> struct QJSValueTraits {
  /**
   * @brief 将JSValueConst转换成c++内置类型
   */
  static T ToNative(JSContext *ctx, JSValueConst v) = delete;

  /**
   * @brief 将c++内置类型转换成JSValue
   */
  static JSValue FromNative(JSContext *ctx, T v) = delete;
};

namespace detail {

/**
 * @brief 解码quickjs数组到c++tuple中
 */
template <typename T, std::size_t Index>
T JSArrayToNativeTupleImpl(JSContext *ctx, JSValueConst v) {
  return QJSValueTraits<T>::ToNative(ctx, v);
}

template <typename Tuple, std::size_t... Is>
Tuple JSArrayToNativeTupleImpl(JSContext *ctx, int argc, JSValueConst *argv,
                               absl::index_sequence<Is...>) {
  return Tuple(
      JSArrayToNativeTupleImpl<typename std::tuple_element<Is, Tuple>::type,
                               Is>(ctx, argv[Is])...);
}

template <typename... Args>
std::tuple<Args...> JSArrayToNativeTuple(JSContext *ctx, int argc,
                                         JSValueConst *argv) {
  using Tuple = std::tuple<typename std::decay<Args>::type...>;
  return JSArrayToNativeTupleImpl<Tuple>(ctx, argc, argv,
                                         absl::index_sequence_for<Args...>());
}

/**
 * @brief 调用c++原生接口，接口参数从JS中获取
 */
template <typename R, typename... Args, typename Callable>
JSValue CallNative(JSContext *ctx, Callable &&call, int argc,
                   JSValueConst *argv) {
  absl::apply(std::forward<Callable>(call),
              JSArrayToNativeTuple<Args...>(ctx, argc, argv));
}

} // namespace detail

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_same<int32_t, T>::value ||
                               std::is_same<int16_t, T>::value ||
                               std::is_same<uint16_t, T>::value>::type> {
  static T ToNative(JSContext *ctx, JSValueConst v) {
    int32_t t;

    if (JS_ToInt32(ctx, &t, v) < 0) {
      throw QJSException();
    }
    return t;
  }

  static JSValue FromNative(JSContext *ctx, T v) { return JS_NewInt32(ctx, v); }
};

template <> struct QJSValueWrapper<uint32_t> {
  static uint32_t ToNative(JSContext *ctx, JSValueConst v) {
    uint32_t t;

    if (JS_ToUint32(ctx, &t, v) < 0) {
      throw QJSException();
    }
    return t;
  }

  static JSValue FromNative(JSContext *ctx, uint32_t v) {
    return JS_NewInt32(ctx, v);
  }
};

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_same<int64_t, T>::value ||
                               std::is_same<uint64_t, T>::value>::type> {
  static T ToNative(JSContext *ctx, JSValueConst v) {
    int64_t t;

    if (JS_ToInt64(ctx, &t, v) < 0) {
      throw QJSException();
    }
    return t;
  }

  static JSValue FromNative(JSContext *ctx, T v) { return JS_NewInt64(ctx, v); }
};

template <typename T>
struct QJSValueTraits<
    T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
  static T ToNative(JSContext *ctx, JSValueConst v) {
    double t;

    if (JS_ToFloat64(ctx, &t, v) < 0) {
      throw QJSException();
    }
    return t;
  }

  static JSValue FromNative(JSContext *ctx, T v) {
    return JS_NewFloat64(ctx, v);
  }
};

template <> struct QJSValueWrapper<std::string> {
  static std::string ToNative(JSContext *ctx, JSValueConst v) {
    size_t len;
    const char *ptr = JS_ToCStringLen(ctx, &len, v);
    if (!ptr)
      throw QJSException();

    return std::string(ptr, len);
  }

  static JSValue FromNative(JSContext *ctx, const std::string v) {
    return JS_NewStringLen(ctx, v.data(), v.size());
  }
};

/**
 * @brief 获取参数个数
 */
template <typename... Args> struct FunctionArgumentNumberTraits {
  static constexpr size_t size = sizeof...(Args);
};

template <typename T, typename = void> struct FunctionValue;

// /**
//  * @brief 类成员函数
//  */
// template <typename R, class T, typename... Args>
// struct FunctionValue<R (T::*)(Args...)> : std::true_type {
//
//   using fn = R (T::*)(Args...);
//   using return_type = R;
//   using class_type = T;
//   using args_type_tuple = std::tuple<Args...>;
//
//   static constexpr size_t args_number =
//       FunctionArgumentNumberTraits<Args...>::size;
//
//   FunctionValue(const char *name_) : name(name_) {}
//
//   const char *name = nullptr;
// };
//

/**
 * @brief 类成员变量
 */
// template <typename M, class T> struct FunctionValue<M T::*> : std::true_type
// {
//   FunctionValue(const char *name_) : name(name_) {}
//
//   using type = M T::*;
//   using class_type = T;
//   using member_type = M;
//
//   const char *name = nullptr;
// };

template <typename T>
struct QJSValueTraits<T,
                       typename std::enable_if<FunctionValue<T>::value>::type> {
  static JSValue FromNative(JSContext *ctx, FunctionValue<T> f) {
    auto func_val = JS_NewCFunction(
        ctx,
        [](JSContext *ctx, JSValueConst this_value, int argc,
           JSValueConst *argv) -> JSValue {
          using R = typename FunctionValue<T>::return_type;
          return CallNative(ctx, FunctionValue<T>::fn, argc, argv);
        },
        f.name, FunctionValue<T>::args_number);
  }
};

class QJSRuntime {
public:
  QJSRuntime() : rt_(JS_NewRuntime()) {}

  ~QJSRuntime() { JS_FreeRuntime(rt_); }

  bool IsJobPending() const { return JS_IsJobPending(rt_); }

private:
  JSRuntime *rt_ = nullptr;

  friend class QJSContext;
};

class QJSModule {
  JSModuleDef *module_ = nullptr;
  JSContext *ctx_ = nullptr;
  std::string name_;
};

class QJSValue {
public:
  QJSValue();

  ~QJSValue();

  QJSValue(const QJSValue &rhs);

  QJSValue(QJSValue &&rhs);

  QJSValue &operator=(const QJSValue &rhs);

  bool operator==(JSValueConst v) const;

  bool operator!=(JSValueConst v) const;

  bool operator==(const QJSValue &other) const;

  bool operator!=(const QJSValue &other) const;

  bool IsError() const;

  bool IsUndefined() const;

  template <typename T> T As() const;

  template <typename T, typename = typename std::enable_if<
                            std::is_member_function_pointer<T>::value>::type>
  void Add(const char *name);

private:
  template <typename T> QJSValue(JSContext *ctx, T &&v);

  JSValue v_{JS_UNDEFINED};
  JSContext *ctx_ = nullptr;

  friend class QJSContext;
};

class QJSContext {
public:
  QJSContext(QJSRuntime &rumtime);

  ~QJSContext();

  template <typename T> QJSValue NewValue(T &&value);

private:
  JSContext *ctx_ = nullptr;
};

inline QJSValue::QJSValue() {}

inline QJSValue::~QJSValue() {
  if (ctx_) {
    JS_FreeValue(ctx_, v_);
  }
}

inline QJSValue::QJSValue(const QJSValue &rhs) {
  ctx_ = rhs.ctx_;
  v_ = JS_DupValue(ctx_, rhs.v_);
}

inline QJSValue::QJSValue(QJSValue &&rhs) {
  std::swap(rhs.ctx_, ctx_);
  std::swap(rhs.v_, v_);
}

inline QJSValue &QJSValue::operator=(const QJSValue &rhs) {
  if (&rhs == this) {
    return *this;
  }

  if (ctx_) {
    JS_FreeValue(ctx_, v_);
  }

  ctx_ = rhs.ctx_;
  v_ = JS_DupValue(ctx_, rhs.v_);

  return *this;
}

inline bool QJSValue::operator==(JSValueConst v) const {
  return JS_VALUE_GET_TAG(v_) == JS_VALUE_GET_TAG(v) &&
         JS_VALUE_GET_PTR(v_) == JS_VALUE_GET_PTR(v);
}

inline bool QJSValue::operator!=(JSValueConst v) const { return !(*this == v); }

inline bool QJSValue::operator==(const QJSValue &other) const {
  return *this == other.v_;
}

inline bool QJSValue::operator!=(const QJSValue &other) const {
  return !(*this == other);
}

inline bool QJSValue::IsError() const { return JS_IsError(ctx_, v_); }

inline bool QJSValue::IsUndefined() const { return JS_IsUndefined(v_); }

template <typename T> T QJSValue::As() const {
  return QJSValueTraits<T>::ToNative(ctx_, v_);
}

template <typename T, typename> void QJSValue::Add(const char *name) {
  QJSValueTraits<T>::FromNative(ctx_, FunctionValue<T>{name});
}

template <typename T> QJSValue::QJSValue(JSContext *ctx, T &&v) : ctx_(ctx) {
  v_ = QJSValueTraits<T>::FromNative(ctx_, std::forward<T>(v));
}

inline QJSContext::QJSContext(QJSRuntime &rumtime)
    : ctx_(JS_NewContext(rumtime.rt_)) {
  JS_SetContextOpaque(ctx_, this);
}

inline QJSContext::~QJSContext() { JS_FreeContext(ctx_); }

template <typename T> QJSValue QJSContext::NewValue(T &&value) {
  return QJSValue(ctx_, std::forward<T>(value));
}
