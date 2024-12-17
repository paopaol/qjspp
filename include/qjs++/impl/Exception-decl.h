#pragma once

#include "qjs++/impl/Value-decl.h"
#include <exception>
#include <string>

namespace qjs {

class Context;
class Exception : public std::exception {
public:
  Exception(JSContext *ctx);

  inline const std::string &String() const { return stack_; }

private:
  std::string stack_;
};
} // namespace qjs
