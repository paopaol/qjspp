#pragma once

#include "qjs++/impl/Runtime.h"
#include "qjs++/impl/Value-decl.h"

namespace qjs {
class Context {
public:
  Context(Runtime &rumtime);

  ~Context();

  Value Global();

  // Value Eval(const std::string &script);

  // Module &Module(const std::string &name);

  // const Module &Module(const std::string &name) const;

  // template <typename T> Class<T> Class(const std::string &name);

  JSContext *Get() const;

private:
  JSContext *ctx_ = nullptr;

  /**
   * @brief 所有我们注册进来的module
   */
  // std::unordered_map<std::string, Module> modules_;

  // template <typename T> friend class Class;
  // friend class Module;
};
} // namespace qjs
