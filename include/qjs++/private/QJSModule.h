#pragma once

#include "qjs++/private/QJSFunction.h"
#include "qjs++/private/QJSWrapper.h"
#include <algorithm>
#include <cassert>
#include <string>

inline QJSModuleExport::QJSModuleExport(JSContext *ctx, JSModuleDef *m,
                                        const std::string &name)
    : ctx_(ctx), m_(m), name_(name) {}

inline QJSModuleExport::~QJSModuleExport() { FreeInternal(); }

inline QJSModuleExport::QJSModuleExport(QJSModuleExport &&other) {
  StealFrom(other);
}

inline QJSModuleExport &QJSModuleExport::operator=(QJSModuleExport &&other) {
  if (this == &other) {
    return *this;
  }
  StealFrom(other);

  return *this;
}

inline void QJSModuleExport::FreeInternal() {
  m_ = nullptr;
  name_.clear();

  if (ctx_) {
    JS_FreeValue(ctx_, v_);
    ctx_ = nullptr;
  }
}

inline void QJSModuleExport::StealFrom(QJSModuleExport &other) {
  FreeInternal();

  ctx_ = other.ctx_;
  m_ = other.m_;
  name_ = std::move(other.name_);

  other.FreeInternal();
}

template <typename Signature, typename F>
QJSModuleExport &QJSModuleExport::Function(F &&f) {
  v_ = QJSValueTraits<QJSLambda<Signature>>::Wrap(ctx_, f);
  JS_AddModuleExport(ctx_, m_, name_.c_str());
  return *this;
}

template <typename... Args>
QJSValue QJSModuleExport::operator()(Args &&...args) {
  QJSValue f(ctx_, JS_DupValue(ctx_, v_));
  return f(std::forward<Args>(args)...);
}

inline QJSModule::QJSModule(QJSContext *ctx, const std::string &name)
    : ctx_(ctx->ctx_), name_(name) {
  m_ = JS_NewCModule(ctx_, name.c_str(), [](JSContext *ctx, JSModuleDef *m) {
    const auto compare =
        [m](const std::pair<const std::string &, const QJSModule &> &module) {
          return module.second.m_ == m;
        };

    auto *context = static_cast<QJSContext *>(JS_GetContextOpaque(ctx));

    auto it = std::find_if(context->modules_.begin(), context->modules_.end(),
                           compare);
    if (it == context->modules_.end()) {
      return -1;
    }

    for (const auto &result : it->second.exports_) {
      const auto &ex = result.second;
      auto ret =
          JS_SetModuleExport(ctx, m, ex.name_.c_str(), JS_DupValue(ctx, ex.v_));
      if (ret != 0) {
        return -1;
      }
    }

    return 0;
  });
  // if (!m_)
  //   throw std::exception{ctx};
  assert(m_);
}

template <typename T> QJSModule &QJSModule::Class(const std::string &name) {
  QJSClass<T>::RegisterClass(ctx_, name);
  return *this;
}

inline QJSModuleExport &QJSModule::operator[](const std::string &name) {
  auto result =
      exports_.insert({name, QJSModuleExport(ctx_, m_, name.c_str())});
  return result.first->second;
}

inline const QJSModuleExport &
QJSModule::operator[](const std::string &name) const {
  return exports_.at(name);
}
