#pragma once

#include "qjs++/impl/Module-decl.h"
#include "qjs++/impl/Runtime.h"
#include "qjs++/impl/Value-decl.h"
#include <unordered_map>

namespace qjs {
class Context {
public:
  Context(Runtime &rumtime);

  ~Context();

  Value Global();

  Value NewObject();

  Value Eval(const std::string &script, bool global = true);

  Module &CreateModule(const std::string &name);

  // const Module &Module(const std::string &name) const;

  // template <typename T> Class<T> Class(const std::string &name);

  JSContext *Get() const;

private:
  JSContext *ctx_ = nullptr;

  /**
   * @brief all created modules
   */
  std::unordered_map<std::string, Module> modules_;

  // template <typename T> friend class Class;
  // friend class Module;

  friend class Module;
};
} // namespace qjs
