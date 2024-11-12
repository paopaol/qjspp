#pragma once

#include "QJSWrapper.h"
#include "qjs++/private/QJSFunction.h"

inline QJSModuleProperty::QJSModuleProperty(JSContext *ctx, JSModuleDef *m,
                                        const std::string &name)
    : ctx_(ctx), m_(m), name_(name) {}

inline QJSModuleProperty::~QJSModuleProperty() { FreeInternal(); }

inline QJSModuleProperty::QJSModuleProperty(QJSModuleProperty &&other) {
  StealFrom(other);
}

inline QJSModuleProperty &QJSModuleProperty::operator=(QJSModuleProperty &&other) {
  if (this == &other) {
    return *this;
  }
  StealFrom(other);

  return *this;
}

inline QJSModuleProperty &QJSModuleProperty::operator=(const QJSValue &other) {
  v_ = QJSValueTraits<QJSValue>::Wrap(ctx_, other);
  JS_AddModuleExport(ctx_, m_, name_.c_str());
  return *this;
}

inline void QJSModuleProperty::FreeInternal() {
  m_ = nullptr;
  name_.clear();

  if (ctx_) {
    JS_FreeValue(ctx_, v_);
    ctx_ = nullptr;
  }
}

inline void QJSModuleProperty::StealFrom(QJSModuleProperty &other) {
  FreeInternal();

  ctx_ = other.ctx_;
  m_ = other.m_;
  name_ = std::move(other.name_);

  other.FreeInternal();
}

template <typename Signature, typename F>
QJSModuleProperty &QJSModuleProperty::Function(F &&f) {
  v_ = QJSValueTraits<QJSLambda<Signature>>::Wrap(ctx_, f);
  JS_AddModuleExport(ctx_, m_, name_.c_str());
  return *this;
}

template <typename... Args>
QJSValue QJSModuleProperty::operator()(Args &&...args) {
  QJSValue f(ctx_, JS_DupValue(ctx_, v_));
  return f(std::forward<Args>(args)...);
}
