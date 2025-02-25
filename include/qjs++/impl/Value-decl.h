#pragma once

#include "quickjs.h"
#include <functional>
#include <type_traits>

namespace qjs {

class Context;

class Value {
public:
  Value();

  Value(Context *ctx);

  Value(Context *ctx, JSValue v);

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<JSValue, T>::value &&
                            !std::is_same<Value, T>::value>::type>
  Value(Context *ctx, T v);

  ~Value();

  Value(Value &&other);

  Value(const Value &) = delete;

  Value &operator=(const Value &) = delete;

  Value &operator=(Value &&other);

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<Value, T>::value>::type>
  Value &operator=(T &&v);

  /**
   * @brief 普通函数绑定
   */
  template <typename R, typename... Args> Value &operator=(R (*f)(Args...));

  /**
   * @brief std::function 绑定
   */
  template <typename R, typename... Args>
  Value &operator=(std::function<R(Args...)> f);

  /**
   * @brief lambda std::function绑定
   */
  template <typename F> Value &SetLambda(std::function<F> f);

  /**
   * @brief 如果Value是一个函数。允许调用这个函数
   */
  template <typename... Args> Value operator()(Args &&...args);

  template <typename T> Value &SetProperty(const std::string &name, T &&v);

  Value Property(const std::string &name) const;

  /**
   * @brief 返回属性
   */
  Value operator[](const std::string &name) const;

  bool IsNull() const;

  bool IsUndefined() const;

  bool IsException() const;

  bool IsBool() const;

  bool IsError() const;

  bool IsArray() const;

  bool IsNumber() const;

  bool IsString() const;

  bool IsBigInt() const;

  bool IsObject() const;

  bool IsSymbol() const;

  bool IsFunction() const;

  std::string ToJsonString() const;

  template <typename T> T As() const;

  JSValue Raw() const;

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<Value, T>::value>::type>
  void Assgin(T &&v);

  template <typename R, typename... Args> void Assgin(R (*f)(Args...));

  template <typename T, typename R, typename... Args>
  void Assgin(R (T::*f)(Args...));

  template <typename R, typename... Args>
  void Assgin(std::function<R(Args...)> f);

private:
  void Steal(Value &&other);

  void FreeInternalValue();

  Context *ctx_ = nullptr;
  JSValue v_;

  friend class Context;
};

} // namespace qjs
