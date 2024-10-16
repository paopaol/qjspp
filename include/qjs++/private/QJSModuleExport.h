#pragma once

#include "QJSWrapper.h"
#include "qjs++/private/QJSFunction.h"

inline QJSModuleExport::QJSModuleExport(JSContext *ctx, JSModuleDef *m,
                                        const std::string &name)
    : ctx_(ctx), m_(m), name_(name) {}

inline QJSModuleExport::~QJSModuleExport() { FreeInternal(); }

inline QJSModuleExport::QJSModuleExport(QJSModuleExport &&other) {
  StealFrom(other);
}

inline QJSModuleExport &QJSModuleExport::operator=(QJSModuleExport &&other) {
  if (this == &other) {
    return *this;
  }
  StealFrom(other);

  return *this;
}

inline QJSModuleExport &QJSModuleExport::operator=(const QJSValue &other) {
  v_ = QJSValueTraits<QJSValue>::Wrap(ctx_, other);
  JS_AddModuleExport(ctx_, m_, name_.c_str());
  return *this;
}

inline void QJSModuleExport::FreeInternal() {
  m_ = nullptr;
  name_.clear();

  if (ctx_) {
    JS_FreeValue(ctx_, v_);
    ctx_ = nullptr;
  }
}

inline void QJSModuleExport::StealFrom(QJSModuleExport &other) {
  FreeInternal();

  ctx_ = other.ctx_;
  m_ = other.m_;
  name_ = std::move(other.name_);

  other.FreeInternal();
}

template <typename Signature, typename F>
QJSModuleExport &QJSModuleExport::Function(F &&f) {
  v_ = QJSValueTraits<QJSLambda<Signature>>::Wrap(ctx_, f);
  JS_AddModuleExport(ctx_, m_, name_.c_str());
  return *this;
}

template <typename... Args>
QJSValue QJSModuleExport::operator()(Args &&...args) {
  QJSValue f(ctx_, JS_DupValue(ctx_, v_));
  return f(std::forward<Args>(args)...);
}
