#pragma once

#include "qjs++/impl/Context-decl.h"
#include "qjs++/impl/Exception-decl.h"
#include <cassert>

namespace qjs {
static const std::string kStdLoadCode = R"---(
import * as std from 'qjs:std';
import * as os from 'qjs:os';

globalThis.std = std;
globalThis.os = os;
)---";

const std::string kBootJsCode = R"---(var exports = {};)---";

inline Context::Context(Runtime &rumtime) : ctx_(JS_NewContext(rumtime.rt_)) {
  JS_SetContextOpaque(ctx_, this);

  js_std_init_handlers(rumtime.rt_);
  js_std_add_helpers(ctx_, 0, nullptr);
  js_init_module_std(ctx_, "std");
  js_init_module_os(ctx_, "os");

  // Eval(kBootJsCode, false);
  // Eval(kStdLoadCode, false);
}

inline Context::~Context() {
  modules_.clear();
  JS_FreeContext(ctx_);
  ctx_ = nullptr;
}

inline Value Context::Global() { return Value(this, JS_GetGlobalObject(ctx_)); }

inline Value Context::NewObject() { return Value(this, JS_NewObject(ctx_)); }

// template <typename T> Class<T> Context::Class(const std::string &name) {
//   Class<T> cls(this, name);
//   return cls;
// }

// inline Value Context::Eval(const std::string &script) {
//   auto ret = JS_Eval(ctx_, script.c_str(), script.size(), "<input>",
//                      JS_EVAL_TYPE_MODULE);
//   return Value(ctx_, ret);
// }
//
// inline Module &Context::Module(const std::string &name) {
//   auto result = modules_.insert({name, Module(this, name)});
//   return result.first->second;
// }

inline Value Context::Eval(const std::string &script, bool global) {
  if (global) {
    auto ret = JS_Eval(ctx_, script.c_str(), script.size(), "<eval>",
                       JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT |
                           JS_EVAL_FLAG_BACKTRACE_BARRIER);
    auto v = Value(this, ret);
    if (v.IsException()) {
      throw Exception(ctx_);
    }
    return v;
  } else {
    auto ret = JS_Eval(ctx_, script.c_str(), script.size(), "<eval>",
                       JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT |
                           JS_EVAL_FLAG_BACKTRACE_BARRIER);
    auto v = Value(this, ret);
    if (v.IsException()) {
      throw Exception(ctx_);
    }
    return v;
  }
}

inline Module &Context::CreateModule(const std::string &name) {
  assert(modules_.find(name) == modules_.end());

  auto result = modules_.insert({name, Module(this, name)});

  return result.first->second;
}

// inline const Module &Context::Module(const std::string &name) const {
//   return modules_.at(name);
// }

inline JSContext *Context::Get() const { return ctx_; }
} // namespace qjs
