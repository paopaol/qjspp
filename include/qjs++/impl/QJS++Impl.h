#pragma once

#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"
#include <functional>
#include <unordered_map>

namespace qjs {
class Runtime {
public:
  Runtime();

  ~Runtime();

  bool IsJobPending() const;

private:
  JSRuntime *rt_ = nullptr;

  friend class Context;
};

class Context {
public:
  Context(Runtime &rumtime);

  ~Context();

  // Value Global();

  // Value Eval(const std::string &script);

  // Module &Module(const std::string &name);

  // const Module &Module(const std::string &name) const;

  // template <typename T> Class<T> Class(const std::string &name);

  JSContext *Get() const;

private:
  JSContext *ctx_ = nullptr;

  /**
   * @brief 所有我们注册进来的module
   */
  // std::unordered_map<std::string, Module> modules_;

  // template <typename T> friend class Class;
  // friend class Module;
};

class Value {
public:
  Value();

  Value(Context *ctx);

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<JSValue, T>::value &&
                            !std::is_same<Value, T>::value>::type>
  Value(Context *ctx, T v)
      : ctx_(ctx), v_(ValueTraits<T>::Wrap(ctx->Get(), std::move(v))) {}

  ~Value();

  Value(Value &&other);

  Value(const Value &) = delete;

  Value &operator=(const Value &) = delete;

  Value &operator=(Value &&other);

  template <typename T, typename = typename std::enable_if<
                            !std::is_same<JSValue, T>::value &&
                            !std::is_same<Value, T>::value>::type>
  Value &operator=(T &&v);

  template <typename R, typename... Args> Value &operator=(R (*f)(Args...));

  template <typename R, typename... Args>
  Value &operator=(std::function<R(Args...)> f);

  template <typename F> Value &SetLambda(std::function<F> f);

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

  bool IsBigFloat() const;

  bool IsFunction() const;

  template <typename T> T As() const;

  const JSValue &Raw() const;

private:
  void Steal(Value &&other);

  void FreeInternalValue();

  Context *ctx_ = nullptr;
  JSValue parent_;
  JSValue v_;

  friend class Context;
};

} // namespace qjs
