#pragma once

#include "qjs++/impl/QJS++Impl.h"

namespace qjs {
inline Context::Context(Runtime &rumtime) : ctx_(JS_NewContext(rumtime.rt_)) {
  JS_SetContextOpaque(ctx_, this);
}

inline Context::~Context() {
  JS_FreeContext(ctx_);
  ctx_ = nullptr;
}

// inline Value Context::Global() { return Value(ctx_,
// JS_GetGlobalObject(ctx_)); }

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

// inline const Module &Context::Module(const std::string &name) const {
//   return modules_.at(name);
// }

inline JSContext *Context::Get() const { return ctx_; }
} // namespace qjs
