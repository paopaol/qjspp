#pragma once

#include "qjs++/impl/Context-decl.h"
#include "qjs++/impl/Exception-decl.h"

namespace qjs {

inline Exception::Exception(JSContext *ctx) {
  auto *context = static_cast<Context *>(JS_GetContextOpaque(ctx));
  auto exc = JS_GetException(ctx);

  bool is_error;
  is_error = JS_IsError(ctx, exc);

  Value ec(context, exc);
  stack_.append(ec.As<std::string>()).append("\n");

  if (!is_error) {
    return;
  }
  stack_.append(ec["stack"].As<std::string>()).append("\n");
}
} // namespace qjs
