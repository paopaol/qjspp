#pragma once

#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>
#include <vector>

class QJSContext;
class QJSRuntime {
public:
  QJSRuntime() : rt_(JS_NewRuntime()) {}

  ~QJSRuntime() { JS_FreeRuntime(rt_); }

  bool IsJobPending() const { return JS_IsJobPending(rt_); }

private:
  JSRuntime *rt_ = nullptr;

  friend class QJSContext;
};

template <typename T> class QJSClass {
public:
  QJSClass &Method();

  /**
   * @brief 将一个类注册到ctx中去
   *
   */
  static void RegisterClass(QJSContext *ctx, const std::string &name);

  /**
   * @brief 为注册的类添加一个构造函数
   */
  template <typename Signature, typename F>
  static void Constructor(QJSContext *ctx, const std::string &name, F &&f);

private:
  QJSContext *ctx_ = nullptr;
  std::string cls_name_;

public:
  static JSClassID cls_id_;
  static JSClassDef cls_def_;
  static std::vector<JSCFunctionListEntry> proto_funcs_;
};

class QJSValueProperty {
public:
  QJSValueProperty(JSContext *ctx, JSValue self, const std::string &name,
                   JSValue v);

  QJSValueProperty(JSContext *ctx, JSValue self, const std::string &name);

  ~QJSValueProperty();

  QJSValueProperty(const QJSValueProperty *other) = delete;

  QJSValueProperty &operator=(const QJSValueProperty &other) = delete;

  template <typename R, typename... Args>
  QJSValueProperty &operator=(R (*f)(Args...));

  QJSValueProperty &operator=(JSValue v);

  template <typename Signature, typename F> QJSValueProperty &Function(F &&f);

  /**
   * @brief 调用
   */
  template <typename... Args> void operator()(Args &&...args);

private:
  JSContext *ctx_ = nullptr;
  JSValue self_;
  std::string name_;
};

class QJSValue {
public:
  QJSValue();

  QJSValue(JSContext *ctx, JSValue v);

  ~QJSValue();

  QJSValue(QJSValue &&other);

  QJSValue(const QJSValue &) = delete;

  QJSValue &operator=(const QJSValue &) = delete;

  QJSValue &operator=(QJSValue &&other);

  bool IsNull() const;

  bool IsUndefined() const;

  QJSValueProperty Property(const std::string &name) const;

  template <typename T> T As() const;

  /**
   * @brief 调用
   */
  template <typename... Args> QJSValue operator()(Args &&...args);

  QJSValueProperty operator[](const std::string &name);

  /**
   * @brief 纯c函数赋值
   */
  template <typename R, typename... Args> QJSValue &operator=(R (*f)(Args...));

private:
  void Steal(QJSValue &&other);

  void FreeInternalValue();

  JSContext *ctx_ = nullptr;
  JSValue v_;

  friend class QJSContext;
};

class QJSModuleExport {
public:
  QJSModuleExport(JSContext *ctx, JSModuleDef *m, const std::string &name);

  ~QJSModuleExport();

  QJSModuleExport(QJSModuleExport &&other);

  QJSModuleExport &operator=(QJSModuleExport &&other);

  QJSModuleExport(const QJSModuleExport &other) = delete;

  QJSModuleExport &operator=(const QJSModuleExport &other) = delete;

  template <typename Signature, typename F> QJSModuleExport &Function(F &&f);

  template <typename... Args> QJSValue operator()(Args &&...args);

private:
  void FreeInternal();

  void StealFrom(QJSModuleExport &other);

  JSContext *ctx_ = nullptr;
  JSModuleDef *m_ = nullptr;
  std::string name_;
  JSValue v_ = JS_UNDEFINED;

  friend class QJSModule;
};

class QJSModule {
public:
  QJSModule(QJSContext *ctx, const std::string &name);

  QJSModule() = default;

  template <typename T> QJSModule &Class(const std::string &name);

  QJSModuleExport &operator[](const std::string &name);

  const QJSModuleExport &operator[](const std::string &name) const;

private:
  JSModuleDef *m_ = nullptr;
  JSContext *ctx_ = nullptr;
  std::string name_;
  std::unordered_map<std::string, QJSModuleExport> exports_;
};

class QJSContext {
public:
  QJSContext(QJSRuntime &rumtime);

  ~QJSContext();

  QJSValue Global();

  QJSValue Eval(const std::string &script);

  QJSModule &Module(const std::string &name);

  const QJSModule &Module(const std::string &name) const;

  template <typename T> QJSClass<T> Class(const std::string &name);

private:
  JSContext *ctx_ = nullptr;

  /**
   * @brief 所有我们注册进来的module
   */
  std::unordered_map<std::string, QJSModule> modules_;

  template <typename T> friend class QJSClass;
  friend class QJSModule;
};
