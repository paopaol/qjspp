#pragma once

#include "QJSCaller.h"
#include "QJSValueTraits.h"

struct QJSCFunction {
  template <typename R, typename... Args>
  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto F = reinterpret_cast<R (*)(Args...)>(opaque);

    return QJSValueTraits<R>::Wrap(
        ctx, InvokeNative<R, Args...>(ctx, F, argc, argv));
  };
};

template <> struct QJSValueTraits<QJSCFunction> {
  template <typename R, typename... Args>
  static JSValue Wrap(JSContext *ctx, R (*F)(Args...)) {
    return JS_NewCClosure(ctx, QJSCFunction::Invoke<R, Args...>, 0, 0,
                          reinterpret_cast<void *>(F), nullptr);
  }
};

template <typename Signature> struct QJSLambda;

template <typename R, typename... Args> struct QJSLambda<R(Args...)> {

  template <typename F> static std::function<R(Args...)> *MakeInvoker(F &&f) {
    return new std::function<R(Args...)>(
        [=](Args &&...args) { return f(std::forward<Args>(args)...); });
  };

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *f = reinterpret_cast<std::function<R(Args...)> *>(opaque);
    return QJSValueTraits<R>::Wrap(
        ctx, InvokeNative<R, Args...>(ctx, *f, argc, argv));
  };

  static void Finalize(void *opaque) {
    auto *f = reinterpret_cast<std::function<R(Args...)> *>(opaque);
    delete f;
  }
};

template <typename Signature> struct QJSValueTraits<QJSLambda<Signature>> {
  static JSValue Wrap(JSContext *ctx, Signature f) {
    auto *ff = QJSLambda<Signature>::MakeInvoker(std::move(f));
    return JS_NewCClosure(ctx, QJSLambda<Signature>::Invoke, 0, 0, (void *)ff,
                          QJSLambda<Signature>::Finalize);
  }
};
