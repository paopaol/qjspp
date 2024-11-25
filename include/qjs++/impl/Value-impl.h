#pragma once

#include "qjs++/impl/Context-decl.h"
#include "qjs++/impl/Value-decl.h"
#include "qjs++/impl/traits/JSValueTraits.h"

namespace qjs {

inline PropertyProxy::PropertyProxy(Context *ctx, Value *v,
                                    const std::string &name)
    : Value(ctx, JS_GetPropertyStr(ctx->Get(), v->Raw(), name.c_str())),
      ctx_(ctx), object_(v), name_(name) {
  if (!object_->IsObject()) {
    *object_ = ctx_->NewObject();
  }
}

template <typename U> inline PropertyProxy &PropertyProxy::operator=(U &&v) {
  Assgin(std::forward<U>(v));

  JS_SetPropertyStr(ctx_->Get(), object_->Raw(), name_.c_str(),
                    JS_DupValue(ctx_->Get(), Raw()));
  return *this;
}

inline Value::Value() : ctx_(nullptr), v_(JS_UNDEFINED) {}

inline Value::Value(Context *ctx) : ctx_(ctx), v_(JS_UNDEFINED) {}

inline Value::Value(Context *ctx, JSValue v) : ctx_(ctx), v_(std::move(v)) {}

template <typename T, typename U>
inline Value::Value(Context *ctx, T v)
    : ctx_(ctx), v_(ValueTraits<T>::Wrap(ctx->Get(), std::move(v))) {}

inline Value::~Value() { FreeInternalValue(); }

template <typename... Args> Value Value::operator()(Args &&...args) {
  JSValue argv[] = {
      ValueTraits<Args>::Wrap(ctx_->Get(), std::forward<Args>(args))...};
  return Value(ctx_,
               JS_Call(ctx_->Get(), v_, JS_UNDEFINED, sizeof...(Args), argv));
}

template <typename T>
inline Value &Value::SetProperty(const std::string &name, T &&v) {
  Value value(ctx_);

  value = std::forward<T>(v);

  JS_SetPropertyStr(ctx_->Get(), v_, name.c_str(),
                    JS_DupValue(ctx_->Get(), value.Raw()));
  return *this;
}

inline Value Value::Property(const std::string &name) const {
  return Value(ctx_, JS_GetPropertyStr(ctx_->Get(), v_, name.c_str()));
}

inline Value Value::operator[](const std::string &name) const {
  return Property(name);
}

inline PropertyProxy Value::operator[](const std::string &name) {
  return PropertyProxy(ctx_, this, name);
}

/**
 * @brief 添加并返回属性
 */
inline bool Value::IsNull() const { return JS_IsNull(v_); }

inline bool Value::IsUndefined() const { return JS_IsUndefined(v_); }

inline bool Value::IsException() const { return JS_IsException(v_); }

inline bool Value::IsBool() const { return JS_IsBool(v_); }

inline bool Value::IsError() const { return JS_IsError(ctx_->Get(), v_); }

inline bool Value::IsArray() const { return JS_IsArray(ctx_->Get(), v_); }

inline bool Value::IsNumber() const { return JS_IsNumber(v_); }

inline bool Value::IsString() const { return JS_IsString(v_); }

inline bool Value::IsBigInt() const { return JS_IsBigInt(ctx_->Get(), v_); }

inline bool Value::IsObject() const { return JS_IsObject(v_); }

inline bool Value::IsSymbol() const { return JS_IsSymbol(v_); }

inline bool Value::IsBigFloat() const { return JS_IsBigFloat(v_); }

inline bool Value::IsFunction() const { return JS_IsFunction(ctx_->Get(), v_); }

template <typename T> T Value::As() const {
  return ValueTraits<T>::Unwrap(ctx_->Get(), v_);
}

inline Value &Value::operator=(Value &&other) {
  if (this == &other) {
    return *this;
  }

  Steal(std::move(other));

  return *this;
}

template <typename T, typename U> Value &Value::operator=(T &&v) {
  Assgin(std::forward<T>(v));
  return *this;
}

template <typename R, typename... Args>
Value &Value::operator=(R (*f)(Args...)) {
  Assgin(f);
  return *this;
}

template <typename R, typename... Args>
Value &Value::operator=(std::function<R(Args...)> f) {
  Assgin(std::move(f));
  return *this;
}

template <typename F> Value &Value::SetLambda(std::function<F> f) {
  FreeInternalValue();

  v_ = ValueTraits<QJSFunction<std::function<F>>>::Wrap(ctx_->Get(),
                                                        std::move(f));
  return *this;
}

inline Value::Value(Value &&other) { Steal(std::move(other)); }

template <typename T, typename U> inline void Value::Assgin(T &&v) {
  FreeInternalValue();
  v_ = ValueTraits<T>::Wrap(ctx_->Get(), std::forward<T>(v));
}

template <typename R, typename... Args> void Value::Assgin(R (*f)(Args...)) {
  FreeInternalValue();
  v_ = ValueTraits<QJSFunction<R (*)(Args...)>>::Wrap(ctx_->Get(), f);
}

template <typename R, typename... Args>
void Value::Assgin(std::function<R(Args...)> f) {
  FreeInternalValue();
  v_ = ValueTraits<QJSFunction<std::function<R(Args...)>>>::Wrap(ctx_->Get(),
                                                                 std::move(f));
}

inline void Value::Steal(Value &&other) {
  if (ctx_) {
    JS_FreeValue(ctx_->Get(), v_);
    ctx_ = nullptr;
  }

  ctx_ = other.ctx_;
  v_ = other.v_;

  other.ctx_ = nullptr;
  other.v_ = JS_UNDEFINED;
}

inline void Value::FreeInternalValue() {
  if (ctx_) {
    JS_FreeValue(ctx_->Get(), v_);
  }
}

inline const JSValue &Value::Raw() const { return v_; }

} // namespace qjs
