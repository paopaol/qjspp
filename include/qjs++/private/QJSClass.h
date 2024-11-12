#pragma once

#include "QJSCaller.h"
#include "QJSValueTraits.h"
#include "QJSWrapper.h"
#include <string>

template <typename T> JSClassID QJSClass<T>::id_ = 0;
template <typename T> JSClassDef QJSClass<T>::def_;

/**
 * @brief 构造函数
 */
template <typename T> struct QJSClassCtor {
  QJSClassCtor(const std::string &name) : name(name) {}

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
    T *inst = static_cast<T *>(JS_GetOpaque(val, QJSClass<T>::id_));
    if (inst) {
      delete inst;
    }
  }

  std::string name;
};

/**
 * @brief 构造函数
 */
template <typename T> struct QJSValueTraits<QJSClassCtor<T>> {
  template <typename... Args>
  static JSValue Wrap(JSContext *ctx, QJSClassCtor<T> ctor) {
    return JS_NewCFunction2(ctx, QJSClassCtor<T>::template Ctor<Args...>,
                            ctor.name.c_str(), sizeof...(Args),
                            JS_CFUNC_constructor, 0);
  }
};

/**
 * @brief 类成员函数
 */
template <typename Signature> struct QJSClassMemberMethod;

template <typename T, typename R, typename... Args>
struct QJSClassMemberMethod<R (T::*)(Args...)> {
  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *method =
        reinterpret_cast<QJSClassMemberMethod<R (T::*)(Args...)> *>(opaque);
    auto f = method->f;
    auto *inst =
        reinterpret_cast<T *>(JS_GetOpaque(this_val, QJSClass<T>::id_));

    return QJSValueTraits<R>::Wrap(ctx, InvokeNative<R, Args...>(
                                            ctx,
                                            [&](Args &&...args) {
                                              auto ret = (inst->*f)(
                                                  std::forward<Args>(args)...);
                                              return ret;
                                            },
                                            argc, argv));
  };

  static void Finalizer(void *opaque) {
    auto *method =
        reinterpret_cast<QJSClassMemberMethod<R (T::*)(Args...)> *>(opaque);
    delete method;
  }

  R (T::*f)(Args...);
};

template <typename T, typename R, typename... Args>
struct QJSClassMemberMethod<R (T::*)(Args...) const> {
  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *method =
        reinterpret_cast<QJSClassMemberMethod<R (T::*)(Args...) const> *>(
            opaque);
    auto f = method->f;
    auto *inst =
        reinterpret_cast<T *>(JS_GetOpaque(this_val, QJSClass<T>::id_));

    return QJSValueTraits<R>::Wrap(ctx, InvokeNative<R, Args...>(
                                            ctx,
                                            [&](Args &&...args) {
                                              return (inst->*f)(
                                                  std::forward<Args>(args)...);
                                            },
                                            argc, argv));
  };

  static void Finalizer(void *opaque) {
    auto *method =
        reinterpret_cast<QJSClassMemberMethod<R (T::*)(Args...) const> *>(
            opaque);
    delete method;
  }

  R (T::*f)(Args...) const;
};

/**
 * @brief 类成员函数
 */
template <typename T> struct QJSValueTraits<QJSClassMemberMethod<T>> {
  template <typename R, typename... Args>
  static JSValue Wrap(JSContext *ctx, R (T::*F)(Args...)) {
    using Signature = R (T::*)(Args...);

    auto *holder = new QJSClassMemberMethod<Signature>;
    holder->f = F;

    return JS_NewCClosure(ctx, QJSClassMemberMethod<Signature>::Invoke, 0, 0,
                          reinterpret_cast<void *>(holder),
                          QJSClassMemberMethod<Signature>::Finalizer);
  }

  template <typename R, typename... Args>
  static JSValue Wrap(JSContext *ctx, R (T::*F)(Args...) const) {
    using Signature = R (T::*)(Args...) const;

    auto *holder = new QJSClassMemberMethod<Signature>;
    holder->f = F;

    return JS_NewCClosure(ctx, QJSClassMemberMethod<Signature>::Invoke, 0, 0,
                          reinterpret_cast<void *>(holder),
                          QJSClassMemberMethod<Signature>::Finalizer);
  }
};
