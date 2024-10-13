#pragma once

#include "QJSWrapper.h"
#include "qjs++/private/QJSFunction.h"

inline QJSValueProperty::QJSValueProperty(JSContext *ctx, JSValue self,
                                          const std::string &name, JSValue v)
    : ctx_(ctx), self_(self), name_(name) {
  JS_SetPropertyStr(ctx_, self_, name_.c_str(), v);
}

inline QJSValueProperty::QJSValueProperty(JSContext *ctx, JSValue self,
                                          const std::string &name)
    : ctx_(ctx), self_(self), name_(name) {}

inline QJSValueProperty::~QJSValueProperty() {
  if (ctx_) {
    JS_FreeValue(ctx_, self_);
  }
}

template <typename R, typename... Args>
QJSValueProperty &QJSValueProperty::operator=(R (*f)(Args...)) {
  JS_SetPropertyStr(ctx_, self_, name_.c_str(),
                    QJSValueTraits<QJSCFunction>::Wrap(ctx_, f));
  return *this;
}

template <typename Signature, typename F>
QJSValueProperty &QJSValueProperty::Function(F &&f) {
  JS_SetPropertyStr(
      ctx_, self_, name_.c_str(),
      QJSValueTraits<QJSLambda<Signature>>::Wrap(ctx_, std::forward<F>(f)));
  return *this;
}

inline QJSValueProperty &QJSValueProperty::operator=(JSValue v) {
  JS_SetPropertyStr(ctx_, self_, name_.c_str(), v);
  return *this;
}

template <typename... Args> void QJSValueProperty::operator()(Args &&...args) {
  QJSValue prop(ctx_, JS_GetPropertyStr(ctx_, self_, name_.c_str()));
  prop(std::forward<Args>(args)...);
}
