#pragma once

#include "qjs++/impl/Class-decl.h"
#include "qjs++/impl/Context-decl.h"
#include "qjs++/impl/traits/FunctionValueClassTraits.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include <cassert>

namespace qjs {

template <typename T> class ClassPrivate {
public:
  Context *ctx = nullptr;
  Module *module = nullptr;
  std::string name;

  /**
   * @brief 构造函数
   */
  Value cls;

  /**
   * @brief 原型
   */
  Value proto;
};

template <typename T>
inline Class<T>::Class(Context *ctx, Module *module, const std::string &name)
    : d(new ClassPrivate<T>) {
  ClassMeta<T>::CreateClass(ctx, name);

  d->ctx = ctx;
  d->module = module;
  d->name = name;

  d->proto = Value(ctx, JS_NewObject(d->ctx->Get()));
  JS_SetClassProto(ctx->Get(), ClassMeta<T>::id,
                   JS_DupValue(d->ctx->Get(), d->proto.Raw()));
}

template <typename T> Class<T>::~Class() {}

template <typename T> Class<T>::Class(Class &&other) { d = std::move(other.d); }

template <typename T> Class<T> &Class<T>::operator=(Class &&other) {
  if (this == &other) {
    return *this;
  }

  d = std::move(other.d);
  return *this;
}

template <typename T>
template <typename... Args>
Class<T> &Class<T>::Construct(const std::string &name) {
  d->cls = Value(d->ctx, ValueTraits<QJSCtor<T *(Args...)>>::Wrap(
                             d->ctx->Get(), QJSCtor<T *(Args...)>{name}));
  JS_SetConstructor(d->ctx->Get(), d->cls.Raw(), d->proto.Raw());

  d->module->SetProperty(name, d->cls.Raw());

  return *this;
}

template <typename T>
template <typename R, typename... Args>
Class<T> &Class<T>::Method(const std::string &name, R (T::*method)(Args...)) {
  d->proto.SetProperty(name, method);

  return *this;
}

} // namespace qjs
