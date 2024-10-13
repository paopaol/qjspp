#pragma once

#include "QJSWrapper.h"
#include <cassert>

inline QJSContext::QJSContext(QJSRuntime &rumtime)
    : ctx_(JS_NewContext(rumtime.rt_)) {
  JS_SetContextOpaque(ctx_, this);
}

inline QJSContext::~QJSContext() {
  JS_FreeContext(ctx_);
  ctx_ = nullptr;
}

inline QJSValue QJSContext::Global() {
  QJSValue value;

  value.v_ = JS_GetGlobalObject(ctx_);
  value.ctx_ = ctx_;

  return value;
}

template <typename T> QJSClass<T> QJSContext::Class(const std::string &name) {
  QJSClass<T> cls(this, name);

  return cls;
}

inline QJSValue QJSContext::Eval(const std::string &script) {
  auto ret = JS_Eval(ctx_, script.c_str(), script.size(), "<input>",
                     JS_EVAL_TYPE_MODULE);
  return QJSValue(ctx_, ret);
}

inline QJSModule &QJSContext::Module(const std::string &name) {
  auto result = modules_.insert({name, QJSModule(this, name)});
  return result.first->second;
}

inline const QJSModule &QJSContext::Module(const std::string &name) const {
  return modules_.at(name);
}
