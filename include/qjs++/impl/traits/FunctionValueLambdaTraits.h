#pragma once

#include "qjs++/impl/Caller.h"
#include "qjs++/impl/traits/FunctionValueFactory.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs.h"
#include <cassert>

namespace qjs {

/**
 * @brief lambda
 */
template <typename R, typename... Args>
struct QJSFunction<std::function<R(Args...)>> {
  using Func = std::function<R(Args...)>;

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto F = reinterpret_cast<QJSFunction<Func> *>(opaque);

    const auto &f = F->f;
    assert(f);

    return ValueTraits<R>::Wrap(ctx,
                                InvokeNative<R, Args...>(ctx, f, argc, argv));
  };

  static void Finalizer(void *opaque) {
    auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete caller;
  }

  Func f;
};

template <typename... Args> struct QJSFunction<std::function<void(Args...)>> {
  using Func = std::function<void(Args...)>;

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto F = reinterpret_cast<QJSFunction<Func> *>(opaque);

    const auto &f = F->f;
    assert(f);

    VoidInvokeNative<Args...>(ctx, f, argc, argv);
    return JS_NULL;
  };

  static void Finalizer(void *opaque) {
    auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete caller;
  }

  Func f;
};

/**
 * @brief lambda
 */
template <typename R, typename... Args>
struct ValueTraits<QJSFunction<std::function<R(Args...)>>> {
  using Func = std::function<R(Args...)>;

  static JSValue Wrap(JSContext *ctx, Func f) {
    auto *caller = new QJSFunction<Func>;
    caller->f = std::move(f);

    return NewClosure(ctx, QJSFunction<Func>::Invoke,
                      QJSFunction<Func>::Finalizer, caller);
  }
};
} // namespace qjs
