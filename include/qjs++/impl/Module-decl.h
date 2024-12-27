#pragma once

#include "qjs++/impl/Class-decl.h"
#include "qjs++/impl/Value-decl.h"
#include <memory>
#include <string>

namespace qjs {

class Context;
class Module {
public:
  Module();

  Module(Context *ctx, const std::string &name);

  ~Module();

  Module(Module &&other);

  Module &operator=(Module &&other);

  Module(const Module &other) = delete;

  Module &operator=(const Module &other) = delete;

  template <typename T> Module &SetProperty(const std::string &name, T &&v);

  template <typename F>
  Module &SetLambdaProperty(const std::string &name, std::function<F> f);

  Value Property(const std::string &name) const;

  template <typename T> Class<T> CreateClass(const std::string &name);

private:
  class Private;
  std::unique_ptr<Private> d;
};

} // namespace qjs
