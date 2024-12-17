#pragma once

#include "qjs++/impl/Context-decl.h"
#include "qjs++/impl/Module-decl.h"
#include "qjs++/impl/Value-decl.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include <algorithm>
#include <cassert>

namespace qjs {

/**
 * @brief module属性辅助类
 */
class ModuleProperty {
public:
  /**
   * @brief 向ctx注册一个module，
   *
   * @name 要注册的module名称
   */
  ModuleProperty(Context *ctx, JSModuleDef *m, const std::string &name)
      : ctx_(ctx), m_(m), name_(name) {
    int a = 0;
  }

  ~ModuleProperty() { FreeInternal(); }

  ModuleProperty(ModuleProperty &&other) { StealFrom(other); }

  ModuleProperty &operator=(ModuleProperty &&other) {
    if (this == &other) {
      return *this;
    }
    StealFrom(other);

    return *this;
  }

  template <typename T> ModuleProperty &operator=(T &&v) {
    v_ = ValueTraits<T>::Wrap(ctx_->Get(), std::forward<T>(v));
    JS_AddModuleExport(ctx_->Get(), m_, name_.c_str());
    return *this;
  }

  Value GetValue() {
    return Value(ctx_, ValueTraits<JSValue>::Wrap(ctx_->Get(), v_));
  }

  ModuleProperty(const ModuleProperty &other) = delete;

  ModuleProperty &operator=(const ModuleProperty &other) = delete;

private:
  void FreeInternal() {
    m_ = nullptr;
    name_.clear();

    if (ctx_) {
      assert(ctx_->Get());
      JS_FreeValue(ctx_->Get(), v_);
      ctx_ = nullptr;
    }
  }

  void StealFrom(ModuleProperty &other) {
    FreeInternal();

    ctx_ = other.ctx_;
    m_ = other.m_;
    name_ = std::move(other.name_);
    v_ = std::move(other.v_);
    // other.FreeInternal();
  }

  Context *ctx_ = nullptr;
  JSModuleDef *m_ = nullptr;
  std::string name_;
  JSValue v_ = JS_UNDEFINED;

  friend class Module;
};

class Module::Private {
public:
  using QJSModuleExportMap = std::unordered_map<std::string, ModuleProperty>;

  Context *ctx = nullptr;
  JSModuleDef *m = nullptr;
  std::string name;

  QJSModuleExportMap exports;
};

inline Module::Module() {}

inline Module::Module(Context *ctx, const std::string &name) : d(new Private) {
  d->ctx = ctx;
  d->name = name;

  d->m = JS_NewCModule(
      d->ctx->Get(), name.c_str(), [](JSContext *ctx, JSModuleDef *m) {
        const auto compare =
            [m](const std::pair<const std::string &, const Module &> &module) {
              return module.second.d->m == m;
            };

        auto *context = static_cast<Context *>(JS_GetContextOpaque(ctx));
        auto &modules = context->modules_;

        auto it = std::find_if(modules.begin(), modules.end(), compare);
        if (it == modules.end()) {
          throw std::runtime_error("invalid module");
        }

        for (const auto &result : it->second.d->exports) {
          const auto &ex = result.second;
          auto ret = JS_SetModuleExport(ctx, m, ex.name_.c_str(),
                                        JS_DupValue(ctx, ex.v_));
          if (ret != 0) {
            throw Exception(ctx);
          }
        }

        return 0;
      });

  if (!d->m) {
    throw Exception(d->ctx->Get());
  }
}

inline Module::~Module() {}

inline Module::Module(Module &&other) { d = std::move(other.d); }

inline Module &Module::operator=(Module &&other) {
  if (this == &other) {
    return *this;
  }

  d = std::move(other.d);
  return *this;
}

template <typename T>
Module &Module::SetProperty(const std::string &name, T &&v) {
  auto result =
      d->exports.insert({name, ModuleProperty(d->ctx, d->m, name.c_str())});
  result.first->second = std::forward<T>(v);
  return *this;
}

inline Value Module::Property(const std::string &name) const {
  auto it = d->exports.find(name);
  return it->second.GetValue();
}

} // namespace qjs
