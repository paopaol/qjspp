#pragma once

#include "qjs++/impl/Caller.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs.h"
#include <cassert>

namespace qjs {

/**
 * @brief mutable none static class::method
 */
template <typename T, typename R, typename... Args>
struct QJSFunction<R (T::*)(Args...)> {
  using Func = R (T::*)(Args...);

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    const auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    auto f = caller->f;
    assert(f);

    // return ValueTraits<R>::Wrap(ctx,
    //                             InvokeNative<R, Args...>(ctx, f, argc,
    //                             argv));
  };

  static void Finalizer(void *opaque) {
    auto *method = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete method;
  }

  Func f;
};

/**
 * @brief immutable none static class::method
 */
template <typename T, typename R, typename... Args>
struct QJSFunction<R (T::*)(Args...) const> {
  using Func = R (T::*)(Args...) const;

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);

    // return JSValueTraits<R>::Wrap(ctx,
    //                               InvokeNative<R, Args...>(ctx, F, argc,
    //                               argv));
  };

  static void Finalizer(void *opaque) {
    auto *method = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete method;
  }

  Func f;
};

/**
 * @brief class::method
 */
template <typename T, typename R, typename... Args>
struct ValueTraits<QJSFunction<R (T::*)(Args...)>> {
  using Func = R (T::*)(Args...);

  static JSValue Wrap(JSContext *ctx, Func f) {
    auto *caller = new QJSFunction<Func>;
    return JS_NewCClosure(ctx, QJSFunction<Func>::Invoke, 0, 0, caller,
                          QJSFunction<Func>::Finalizer);
  }
};

/**
 * @brief class::const methos
 */
template <typename T, typename R, typename... Args>
struct ValueTraits<QJSFunction<R (T::*)(Args...) const>> {
  using Func = R (T::*)(Args...) const;

  static JSValue Wrap(JSContext *ctx, Func f) {
    auto *caller = new QJSFunction<Func>;
    return JS_NewCClosure(ctx, QJSFunction<Func>::Invoke, 0, 0, caller,
                          QJSFunction<Func>::Finalizer);
  }
};
} // namespace qjs
