#pragma once

#include "QJSWrapper.h"
#include "quickjs/quickjs-libc.h"

static JSContext *JS_NewCustomContext(JSRuntime *rt) {
  JSContext *ctx;
  ctx = JS_NewContext(rt);
  if (!ctx)

    return NULL;
#ifdef CONFIG_BIGNUM

  if (bignum_ext) {
    JS_AddIntrinsicBigFloat(ctx);
    JS_AddIntrinsicBigDecimal(ctx);

    JS_AddIntrinsicOperators(ctx);
    JS_EnableBignumExt(ctx, TRUE);
  }
#endif
  /* system modules */
  //js_init_module_std(ctx, "std");
  //js_init_module_os(ctx, "os");
  return ctx;
}

inline QJSRuntime::QJSRuntime() : rt_(JS_NewRuntime()) {
  // js_std_set_worker_new_context_func(JS_NewCustomContext);
  // js_std_init_handlers(rt_);
}

inline QJSRuntime::~QJSRuntime() { JS_FreeRuntime(rt_); }

inline bool QJSRuntime::IsJobPending() const { return JS_IsJobPending(rt_); }
