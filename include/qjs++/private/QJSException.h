#pragma once

#include "qjs++/private/QJSWrapper.h"

#include <exception>

class QJSException : public std::exception {
public:
  QJSException(JSContext *ctx) {
    QJSValue ec(ctx, JS_GetException(ctx));
    stack_ = ec["stack"].Value().As<std::string>();
  }

  const std::string &String() const { return stack_; }

private:
  std::string stack_;
};
