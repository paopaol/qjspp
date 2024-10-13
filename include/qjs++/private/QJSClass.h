#pragma once

#include "QJSCaller.h"
#include "QJSValueTraits.h"
#include "QJSWrapper.h"
#include <cassert>
#include <string.h>
#include <string>

template <typename T> struct QJSClassHelper {
  QJSClassHelper(const char *name) : name(name) {}

  /**
   * @brief 构造
   */
  template <typename... Args>
  static JSValue Ctor(JSContext *ctx, JSValueConst new_target, int argc,
                      JSValueConst *argv) {
    auto *inst = InvokeNative<T *, Args...>(
        ctx, [](Args &&...args) { return new T(std::forward<Args>(args)...); },
        argc, argv);

    return QJSValueTraits<T *>::Wrap(ctx, inst);
  }
  /**
   * @brief 析构
   */
  static void Finalizer(JSRuntime *rt, JSValue val) {
    T *inst = static_cast<T *>(JS_GetOpaque(val, QJSClass<T>::cls_id_));
    if (inst) {
      delete inst;
    }
  }

  const char *name = nullptr;
};

template <typename T> struct QJSValueTraits<QJSClassHelper<T>> {
  template <typename... Args>
  static JSValue Wrap(JSContext *ctx, QJSClassHelper<T> ctor) {
    return JS_NewCFunction2(ctx, QJSClassHelper<T>::template Ctor<Args...>,
                            ctor.name, sizeof...(Args), JS_CFUNC_constructor,
                            0);
  }
};

/*****************************
 *
 *
 *
 ********************************/
template <typename T> JSClassID QJSClass<T>::cls_id_ = 0;
template <typename T> JSClassDef QJSClass<T>::cls_def_;
template <typename T>
std::vector<JSCFunctionListEntry> QJSClass<T>::proto_funcs_;

template <typename T> struct ClassMethod {
  template <typename R, typename... Args> struct Method {
    R (T::*F)(Args...);
  };

  char *name = nullptr;

  template <typename R, typename... Args>
  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto method = reinterpret_cast<Method<R, Args...> *>(opaque);
    auto f = method->F;
    auto *inst =
        reinterpret_cast<T *>(JS_GetOpaque(this_val, QJSClass<T>::cls_id_));

    return QJSValueTraits<R>::Wrap(ctx, InvokeNative<R, Args...>(
                                            ctx,
                                            [&](Args &&...args) {
                                              return (inst->*f)(
                                                  std::forward<Args>(args)...);
                                            },
                                            argc, argv));
  }
};

template <typename T> struct QJSValueTraits<ClassMethod<T>> {
  template <typename R, typename... Args>
  static JSValue Wrap(JSContext *ctx, R (T::*F)(Args...)) {
    auto *holder = new typename ClassMethod<T>::template Method<R, Args...>;
    holder->F = F;
    return JS_NewCClosure(ctx, ClassMethod<T>::template Invoke<R, Args...>, 0,
                          0, reinterpret_cast<void *>(holder), nullptr);
  }
};

template <typename T>
void QJSClass<T>::RegisterClass(QJSContext *ctx, const std::string &name) {
  if (cls_id_ != 0) {
    return;
  }

  cls_def_.class_name = strdup(name.c_str());
  cls_def_.finalizer = QJSClassHelper<T>::Finalizer;

  JS_NewClassID(&cls_id_);
  assert(cls_id_ >= 0);
  JS_NewClass(JS_GetRuntime(ctx->ctx_), cls_id_, nullptr);

  auto proto = JS_NewObject(ctx->ctx_);
  JS_SetPropertyFunctionList(ctx->ctx_, proto, proto_funcs_.data(),
                             proto_funcs_.size());
}

template <typename T>
template <typename Signature, typename F>
void QJSClass<T>::Constructor(QJSContext *ctx, const std::string &name, F &&f) {
  // auto cls = QJSValueTraits<ClassConstructor<T>>::Wrap(
  //     ctx->ctx_, ClassConstructor<T>(name, f));

  // JS_SetConstructor(ctx->ctx_, cls, proto);
}
