#pragma once

#include "qjs++/impl/Caller.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs/quickjs.h"
#include <cassert>

namespace qjs {

template <typename T> struct QJSFunction;

/**
 * @brief c function or static class::method
 */
template <typename R, typename... Args> struct QJSFunction<R (*)(Args...)> {
  using Func = R (*)(Args...);

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *f = reinterpret_cast<R (*)(Args...)>(opaque);

    return ValueTraits<R>::Wrap(ctx,
                                InvokeNative<R, Args...>(ctx, f, argc, argv));
  };
};

template <typename... Args> struct QJSFunction<void (*)(Args...)> {
  using Func = void (*)(Args...);

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *f = reinterpret_cast<Func>(opaque);
    InvokeNative<void, Args...>(ctx, f, argc, argv);

    return JS_NULL;
  };

  Func f;
};

/**
 * @brief c function or static class::method
 */
template <typename R, typename... Args>
struct ValueTraits<QJSFunction<R (*)(Args...)>> {
  using Func = R (*)(Args...);

  static JSValue Wrap(JSContext *ctx, Func f) {
    return JS_NewCClosure(ctx, QJSFunction<Func>::Invoke, 0, 0,
                          reinterpret_cast<void *>(f), nullptr);
  }
};
} // namespace qjs
