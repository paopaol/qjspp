#pragma once

#include "qjs++/impl/Caller.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include "quickjs.h"
#include <cassert>
#include <mutex>

namespace qjs {

class ClassIdGenerator {
public:
  static JSClassID Next() {
    static std::mutex mutex;
    static JSClassID next = 1000;

    std::lock_guard<std::mutex> l(mutex);
    return next++;
  }
};

template <typename T> struct ClassMeta {
  static JSClassID id;
  static JSClassDef def;
  static std::once_flag once;

  static void finalizer(JSRuntime *rt, JSValue val) {
    auto *opa = static_cast<std::shared_ptr<T> *>(JS_GetOpaque(val, id));
    if (opa) {
      delete opa->get();
      delete opa;
    }
  };

  static void CreateClass(Context *ctx, const std::string &name) {
    std::call_once(once, [&]() {
      id = ClassIdGenerator::Next();
      def.class_name = strdup(name.c_str());
      def.finalizer = finalizer;
    });

    auto *rt = JS_GetRuntime(ctx->Get());
    if (JS_IsRegisteredClass(rt, id)) {
      return;
    }

    JS_NewClass(JS_GetRuntime(ctx->Get()), id, &def);
  }
};

template <typename T> JSClassID ClassMeta<T>::id = 0;
template <typename T> JSClassDef ClassMeta<T>::def{};
template <typename T> std::once_flag ClassMeta<T>::once;

class ClosureClass {
public:
  using JSCallback = JSValue (*)(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv, int magic,
                                 void *opaque);
  using JSCallbackFinalizer = void (*)(void *opaque);

  ClosureClass(JSCallback callback, JSCallbackFinalizer finalizer, void *opaque)
      : callback_(callback), finalizer_(finalizer), opaque_(opaque) {}

  ~ClosureClass() {
    if (finalizer_) {
      finalizer_(opaque_);
    }
  }

  JSValue Call(JSContext *ctx, JSValueConst this_val, int argc,
               JSValueConst *argv, int magic) {
    try {
      return callback_(ctx, this_val, argc, argv, magic, opaque_);
    } catch (const Exception &e) {
      abort();
      // return qjs_backend::throwException(e, engine);
    }
    return JS_NULL;
  }

private:
  JSCallback callback_ = nullptr;
  JSCallbackFinalizer finalizer_ = nullptr;
  void *opaque_ = nullptr;
};

template <typename T>
struct ValueTraits<T *, std::enable_if_t<std::is_class<T>::value>> {
  static T *Unwrap(JSContext *ctx, JSValueConst v) {
    auto *opa =
        static_cast<std::shared_ptr<T> *>(JS_GetOpaque(v, ClassMeta<T>::id));
    return opa->get();
  }

  static JSValue Wrap(JSContext *ctx, T *v) {
    assert(ClassMeta<T>::id != 0);
    auto inst = JS_NewObjectClass(ctx, ClassMeta<T>::id);
    if (JS_IsException(inst)) {
      return inst;
    }
    std::shared_ptr<T> *opaque = new std::shared_ptr<T>(v, [](T *) {});
    JS_SetOpaque(inst, opaque);
    return inst;
  }
};

inline JSValue MakeOpaqueValue(JSContext *ctx,
                               ClosureClass::JSCallback callback,
                               ClosureClass::JSCallbackFinalizer finalizer,
                               void *opaque) {
  auto *cls = new ClosureClass(callback, finalizer, opaque);
  return ValueTraits<ClosureClass *>::Wrap(ctx, cls);
}

inline JSValue NewClosure(JSContext *ctx, ClosureClass::JSCallback callback,
                          ClosureClass::JSCallbackFinalizer finalizer,
                          void *opaque) {
  JSValue data = MakeOpaqueValue(ctx, callback, finalizer, opaque);

  const auto func = [](JSContext *ctx, JSValueConst this_val, int argc,
                       JSValueConst *argv, int magic, JSValue *func_data) {
    ClosureClass *data = ValueTraits<ClosureClass *>::Unwrap(ctx, func_data[0]);
    return data->Call(ctx, this_val, argc, argv, magic);
  };

  auto cf = JS_NewCFunctionData(ctx, func, 0, 0, 1, &data);
  JS_FreeValue(ctx, data);

  return cf;
}

/**
 * @brief class constructor
 */
template <typename T, typename... Args> struct QJSCtor<T *(Args...)> {
  static JSValue Invoke(JSContext *ctx, JSValue this_val, int argc,
                        JSValue *argv) {
    T *ret = InvokeNative<T *, Args...>(
        ctx, [](Args... args) { return new T(args...); }, argc, argv);
    return ValueTraits<T *>::Wrap(ctx, ret);
  };

  std::string name;
};

template <typename T, typename... Args>
struct ValueTraits<QJSCtor<T *(Args...)>> {
  static JSValue Wrap(JSContext *ctx, QJSCtor<T *(Args...)> ctor) {
    return JS_NewCFunction2(ctx, QJSCtor<T *(Args...)>::Invoke,
                            ctor.name.c_str(), sizeof...(Args),
                            JS_CFUNC_constructor, 0);
  }
};

/**
 * @brief mutable none static class::method
 */
template <typename T, typename R, typename... Args>
struct QJSFunction<R (T::*)(Args...)> {
  using Func = R (T::*)(Args...);

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    const auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    assert(caller && caller->f);
    auto f = caller->f;

    auto *inst = ValueTraits<T *>::Unwrap(ctx, this_val);
    assert(inst);

    return ValueTraits<R>::Wrap(ctx, InvokeNative<R, Args...>(
                                         ctx,
                                         [&](Args &&...args) {
                                           auto ret = (inst->*f)(
                                               std::forward<Args>(args)...);
                                           return ret;
                                         },
                                         argc, argv));
  };

  static void Finalizer(void *opaque) {
    auto *method = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete method;
  }

  Func f;
};

/**
 * @brief mutable void return none static class::method
 */
template <typename T, typename... Args>
struct QJSFunction<void (T::*)(Args...)> {
  using Func = void (T::*)(Args...);

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    const auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    assert(caller && caller->f);

    // VoidInvokeNative<Args...>(ctx, caller->f, argc, argv);
    return JS_NULL;
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
    assert(caller && caller->f);
    auto f = caller->f;

    auto *inst =
        reinterpret_cast<T *>(JS_GetOpaque(this_val, ClassMeta<T>::id));
    assert(inst);

    return ValueTraits<R>::Wrap(ctx, InvokeNative<R, Args...>(
                                         ctx,
                                         [&](Args &&...args) {
                                           auto ret = (inst->*f)(
                                               std::forward<Args>(args)...);
                                           return ret;
                                         },
                                         argc, argv));
  };

  static void Finalizer(void *opaque) {
    auto *method = reinterpret_cast<QJSFunction<Func> *>(opaque);
    delete method;
  }

  Func f;
};

/**
 * @brief immutable void return none static class::method
 */
template <typename T, typename... Args>
struct QJSFunction<void (T::*)(Args...) const> {
  using Func = void (T::*)(Args...) const;

  static JSValue Invoke(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv, int magic, void *opaque) {
    auto *caller = reinterpret_cast<QJSFunction<Func> *>(opaque);
    assert(caller && caller->f);

    // VoidInvokeNative<Args...>(ctx, caller->f, argc, argv);
    return JS_NULL;
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
    caller->f = f;
    return NewClosure(ctx, QJSFunction<Func>::Invoke,
                      QJSFunction<Func>::Finalizer, caller);
  }
};

/**
 * @brief class::const method
 */
template <typename T, typename R, typename... Args>
struct ValueTraits<QJSFunction<R (T::*)(Args...) const>> {
  using Func = R (T::*)(Args...) const;

  static JSValue Wrap(JSContext *ctx, Func f) {
    auto *caller = new QJSFunction<Func>;
    caller->f = f;
    return NewClosure(ctx, QJSFunction<Func>::Invoke,
                      QJSFunction<Func>::Finalizer, caller);
  }
};

} // namespace qjs
