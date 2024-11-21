#pragma once

#include "quickjs/quickjs.h"

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

} // namespace qjs
