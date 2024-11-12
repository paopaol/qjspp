#pragma once

#include "QJSWrapper.h"
#include "qjs++/private/QJSClass.h"
#include "quickjs/quickjs.h"
#include "string.h"
#include <cassert>
#include <vector>

template <typename T> class QJSClassExportor<T>::Private {
public:
  JSContext *ctx;
  QJSModule *module = nullptr;

  /**
   * @brief 构造函数
   */
  std::unique_ptr<QJSValue> ctor;

  /**
   * @brief 原型
   */
  std::unique_ptr<QJSValue> proto;

  static std::vector<JSCFunctionListEntry> protos;
};

template <typename T>
QJSClassExportor<T>::QJSClassExportor() : d(new Private) {}

template <typename T>
QJSClassExportor<T>::QJSClassExportor(JSContext *ctx, QJSModule *module,
                                      JSValue proto)
    : d(new Private) {
  d->ctx = ctx;
  d->module = module;
  d->proto = std::make_unique<QJSValue>(ctx, proto);
}

template <typename T> QJSClassExportor<T>::~QJSClassExportor() {}

template <typename T>
QJSClassExportor<T>::QJSClassExportor(QJSClassExportor &&other)
    : d(std::move(other.d)) {}

template <typename T>
QJSClassExportor<T> &QJSClassExportor<T>::operator=(QJSClassExportor &&other) {
  d = std::move(other.d);
}

template <typename T>
QJSClassExportor<T> QJSClassExportor<T>::New(QJSModule *module, JSContext *ctx,
                                             const std::string &name) {
  using Class = QJSClass<T>;

  if (Class::id_ != 0) {
    abort();
    return QJSClassExportor();
  }

  Class::def_.class_name = strdup(name.c_str());
  Class::def_.finalizer = QJSClassCtor<T>::Finalizer;

  JS_NewClassID(&Class::id_);
  assert(Class::id_ >= 0);
  JS_NewClass(JS_GetRuntime(ctx), Class::id_, &Class::def_);

  auto proto = JS_NewObject(ctx);
  JS_SetClassProto(ctx, Class::id_, proto);

  return QJSClassExportor(ctx, module, JS_DupValue(ctx, proto));
}

template <typename T>
template <typename... Args>
QJSClassExportor<T> &QJSClassExportor<T>::Construct(const std::string &name) {
  d->ctor = std::make_unique<QJSValue>(
      d->ctx, QJSValueTraits<QJSClassCtor<T>>::template Wrap<Args...>(
                  d->ctx, QJSClassCtor<T>(name)));
  JS_SetConstructor(d->ctx, d->ctor->Raw(), d->proto->Raw());

  (*d->module)[name] = *d->ctor;
  return *this;
}

template <typename T>
template <typename F>
QJSClassExportor<T> &QJSClassExportor<T>::Method(const std::string &name,
                                                 F &&f) {
  (*d->proto)[name] =
      QJSValueTraits<QJSClassMemberMethod<T>>::Wrap(d->ctx, std::forward<F>(f));

  return *this;
}
