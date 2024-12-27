#pragma once

#include <memory>
#include <string>

namespace qjs {

class Context;
class Module;

template <typename T> class ClassPrivate;
template <typename T> class Class {
public:
  Class(Context *ctx, Module *module, const std::string &name);

  ~Class();

  Class(const Class &other) = delete;

  Class &operator=(const Class &other) = delete;

  Class(Class &&other);

  Class &operator=(Class &&other);

  template <typename... Args> Class &Construct(const std::string &name);

  template <typename R, typename... Args>
  Class &Method(const std::string &name, R (T::*method)(Args...));

private:
  std::unique_ptr<ClassPrivate<T>> d;
};
} // namespace qjs
