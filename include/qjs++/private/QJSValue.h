#pragma once

#include "QJSFunction.h"
#include "QJSWrapper.h"
#include "quickjs/quickjs.h"

inline QJSValue::QJSValue() : ctx_(nullptr), v_(JS_UNDEFINED) {}

inline QJSValue::QJSValue(JSContext *ctx, JSValue v) : ctx_(ctx), v_(v) {}

inline QJSValue::~QJSValue() { FreeInternalValue(); }

inline bool QJSValue::IsNull() const { return JS_IsNull(v_); }

inline bool QJSValue::IsUndefined() const { return JS_IsUndefined(v_); }

inline QJSValueProperty QJSValue::Property(const std::string &name) const {
  return QJSValueProperty(ctx_, JS_DupValue(ctx_, v_), name);
}

template <typename T> T QJSValue::As() const {
  return QJSValueTraits<T>::Unwrap(ctx_, v_);
}

template <typename... Args> QJSValue QJSValue::operator()(Args &&...args) {
  JSValue argv[] = {QJSValueTraits<Args>::Wrap(ctx_, args)...};
  auto rt = JS_Call(ctx_, v_, JS_UNDEFINED, sizeof...(Args), argv);

  return QJSValue(ctx_, rt);
}

inline QJSValueProperty QJSValue::operator[](const std::string &name) {
  return QJSValueProperty(ctx_, JS_DupValue(ctx_, v_), name);
}

inline QJSValue &QJSValue::operator=(QJSValue &&other) {
  if (this == &other) {
    return *this;
  }

  Steal(std::move(other));

  return *this;
}

template <typename R, typename... Args>
QJSValue &QJSValue::operator=(R (*f)(Args...)) {
  FreeInternalValue();
  v_ = QJSValueTraits<QJSCFunction>::Wrap(ctx_, f);
  return *this;
}

inline QJSValue::QJSValue(QJSValue &&other) { Steal(std::move(other)); }

inline void QJSValue::Steal(QJSValue &&other) {
  if (ctx_) {
    JS_FreeValue(ctx_, v_);
    ctx_ = nullptr;
  }

  ctx_ = other.ctx_;
  v_ = other.v_;

  other.ctx_ = nullptr;
  other.v_ = JS_UNDEFINED;
}

inline void QJSValue::FreeInternalValue() {
  if (ctx_) {
    JS_FreeValue(ctx_, v_);
  }
}

inline const JSValue &QJSValue::Raw() const { return v_; }
