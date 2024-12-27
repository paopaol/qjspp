#pragma once

#include "qjs++/impl/Module-decl.h"
#include "qjs++/impl/Value-decl.h"
#include <unordered_map>

namespace qjs {
class Runtime;
class Context {
public:
  Context(Runtime &runtime);

  ~Context();

  Value Global();

  Value NewObject();

  Value Eval(const std::string &script, bool global = true);

  Module &CreateModule(const std::string &name);

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
