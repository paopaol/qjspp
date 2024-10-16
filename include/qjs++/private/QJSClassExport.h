#pragma once

#include "QJSWrapper.h"
#include "qjs++/private/QJSClass.h"
#include "quickjs/quickjs.h"
#include "string.h"
#include <cassert>
#include <vector>

template <typename T> class QJSClassExport<T>::Private {
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
  static std::vector<JSCFunctionListEntry> protos_;
};

template <typename T> QJSClassExport<T>::QJSClassExport() : d(new Private) {}

template <typename T> QJSClassExport<T>::~QJSClassExport() {}

template <typename T>
QJSClassExport<T>::QJSClassExport(QJSClassExport &&other)
    : d(std::move(other.d)) {}

template <typename T>
QJSClassExport<T> &QJSClassExport<T>::operator=(QJSClassExport &&other) {
  d = std::move(other.d);
}

template <typename T>
QJSClassExport<T> QJSClassExport<T>::Export(QJSModule *module, JSContext *ctx,
                                            const std::string &name) {
  using Class = QJSClass<T>;

  if (Class::id_ != 0) {
    return QJSClassExport();
  }

  Class::def_.class_name = strdup(name.c_str());
  Class::def_.finalizer = QJSClassCtor<T>::Finalizer;

  JS_NewClassID(&Class::id_);
  assert(Class::id_ >= 0);
  JS_NewClass(JS_GetRuntime(ctx), Class::id_, &Class::def_);

  auto proto = JS_NewObject(ctx);
  JS_SetClassProto(ctx, Class::id_, proto);

  QJSClassExport ex;

  ex.d->ctx = ctx;
  ex.d->module = module;
  ex.d->proto = std::make_unique<QJSValue>(ctx, JS_DupValue(ctx, proto));

  return ex;
}

template <typename T>
template <typename... Args>
QJSClassExport<T> &QJSClassExport<T>::Construct(const std::string &name) {
  d->ctor = std::make_unique<QJSValue>(
      d->ctx, QJSValueTraits<QJSClassCtor<T>>::template Wrap<Args...>(
                  d->ctx, QJSClassCtor<T>(name)));
  JS_SetConstructor(d->ctx, d->ctor->Raw(), d->proto->Raw());

  (*d->module)[name] = *d->ctor;
  return *this;
}

template <typename T>
template <typename F>
QJSClassExport<T> &QJSClassExport<T>::Method(const std::string &name, F &&f) {
  (*d->proto)[name] =
      QJSValueTraits<QJSClassMemberMethod<T>>::Wrap(d->ctx, std::forward<F>(f));

  return *this;
}
