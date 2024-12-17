#pragma once

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

  Value Property(const std::string &name) const;

private:
  class Private;
  std::unique_ptr<Private> d;
};

} // namespace qjs
