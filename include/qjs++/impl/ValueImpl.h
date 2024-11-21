#pragma once

#include "qjs++/impl/QJS++Impl.h"
#include "qjs++/impl/traits/JSValueTraits.h"

namespace qjs {
inline Value::Value()
    : ctx_(nullptr), parent_(JS_UNDEFINED), v_(JS_UNDEFINED) {}

inline Value::Value(Context *ctx)
    : ctx_(ctx), parent_(JS_UNDEFINED), v_(JS_UNDEFINED) {}

inline Value::~Value() { FreeInternalValue(); }

inline bool Value::IsNull() const { return JS_IsNull(v_); }

inline bool Value::IsUndefined() const { return JS_IsUndefined(v_); }

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

template <typename T, typename U> Value &Value::operator=(T v) {
  FreeInternalValue();
  v_ = ValueTraits<T>::Wrap(ctx_->Get(), std::move(v));
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
