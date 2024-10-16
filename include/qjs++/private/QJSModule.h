#pragma once

#include <algorithm>
#include <cassert>
#include <string>

#include "qjs++/private/QJSException.h"
#include "qjs++/private/QJSFunction.h"
#include "qjs++/private/QJSWrapper.h"

class QJSModule::Private {
 public:
  JSModuleDef *m = nullptr;
  JSContext *ctx = nullptr;
  std::string name;
  std::unordered_map<std::string, QJSModuleExport> exports;
};

inline QJSModule::QJSModule(QJSContext *ctx, const std::string &name)
    : d(new Private) {
  d->ctx = ctx->ctx_;
  d->name = name;
  d->m =
      JS_NewCModule(d->ctx, name.c_str(), [](JSContext *ctx, JSModuleDef *m) {
        const auto compare =
            [m](const std::pair<const std::string &, const QJSModule &>
                    &module) { return module.second.d->m == m; };

        auto *context = static_cast<QJSContext *>(JS_GetContextOpaque(ctx));

        auto it = std::find_if(context->modules_.begin(),
                               context->modules_.end(), compare);
        if (it == context->modules_.end()) {
          return -1;
        }

        for (const auto &result : it->second.d->exports) {
          const auto &ex = result.second;
          auto ret = JS_SetModuleExport(ctx, m, ex.name_.c_str(),
                                        JS_DupValue(ctx, ex.v_));
          if (ret != 0) {
            throw QJSException(ctx);
            return -1;
          }
        }

        return 0;
      });
  if (!d->m) {
    throw QJSException(d->ctx);
  }
}

template <typename T>
QJSClassExport<T> QJSModule::Class(const std::string &name) {
  auto v = QJSClassExport<T>::Export(this, d->ctx, name);
  return v;
}

inline QJSModuleExport &QJSModule::operator[](const std::string &name) {
  return Export(name);
}

inline const QJSModuleExport &QJSModule::operator[](
    const std::string &name) const {
  return d->exports.at(name);
}

QJSModuleExport &QJSModule::Export(const std::string &name) {
  auto result =
      d->exports.insert({name, QJSModuleExport(d->ctx, d->m, name.c_str())});
  return result.first->second;
}
