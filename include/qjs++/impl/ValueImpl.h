#pragma once

#include "qjs++/impl/QJS++Impl.h"
#include "qjs++/impl/traits/FunctionValueTraits.h"
#include "qjs++/impl/traits/JSValueTraits.h"

namespace qjs {
inline Value::Value()
    : ctx_(nullptr), parent_(JS_UNDEFINED), v_(JS_UNDEFINED) {}

inline Value::Value(Context *ctx)
    : ctx_(ctx), parent_(JS_UNDEFINED), v_(JS_UNDEFINED) {}

inline Value::~Value() { FreeInternalValue(); }

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
  FreeInternalValue();
  v_ = ValueTraits<T>::Wrap(ctx_->Get(), std::forward<T>(v));
  return *this;
}

template <typename R, typename... Args>
Value &Value::operator=(R (*f)(Args...)) {
  FreeInternalValue();
  v_ = ValueTraits<QJSFunction<R (*)(Args...)>>::Wrap(ctx_->Get(), f);
  return *this;
}

template <typename F> Value &Value::SetLambda(std::function<F> f) {
  FreeInternalValue();

  v_ = ValueTraits<QJSFunction<std::function<F>>>::Wrap(ctx_->Get(),
                                                        std::move(f));
  return *this;
}

inline Value::Value(Value &&other) { Steal(std::move(other)); }

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
