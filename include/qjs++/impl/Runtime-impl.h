#pragma once

#include "qjs++/impl/Runtime-decl.h"
#include "quickjs-libc.h"

namespace qjs {

inline Runtime::Runtime() : rt_(JS_NewRuntime()) {
  JS_SetModuleLoaderFunc(rt_, nullptr, js_module_loader, nullptr);
  js_std_init_handlers(rt_);
}

inline Runtime::~Runtime() {
  js_std_free_handlers(rt_);

  JS_FreeRuntime(rt_);
}

inline bool Runtime::IsJobPending() const { return JS_IsJobPending(rt_); }
} // namespace qjs
