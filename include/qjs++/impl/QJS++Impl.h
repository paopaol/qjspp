#pragma once

#include "quickjs/quickjs.h"
#include <unordered_map>

namespace qjs {
class Runtime {
public:
  Runtime();

  ~Runtime();

  bool IsJobPending() const;

private:
  JSRuntime *rt_ = nullptr;

  friend class Context;
};

class Context {
public:
  Context(Runtime &rumtime);

  ~Context();

  // QJSValue Global();

  // QJSValue Eval(const std::string &script);

  // QJSModule &Module(const std::string &name);

  // const QJSModule &Module(const std::string &name) const;

  // template <typename T> QJSClass<T> Class(const std::string &name);

private:
  JSContext *ctx_ = nullptr;

  /**
   * @brief 所有我们注册进来的module
   */
  // std::unordered_map<std::string, QJSModule> modules_;

  // template <typename T> friend class QJSClass;
  // friend class QJSModule;
};

} // namespace qjs
