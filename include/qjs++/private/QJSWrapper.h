#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "quickjs/quickjs.h"

class QJSContext;
class QJSModule;
class QJSValue;

template <typename T> class QJSClassExport {
public:
  QJSClassExport();

  ~QJSClassExport();

  QJSClassExport(QJSClassExport &&other);

  QJSClassExport &operator=(QJSClassExport &&other);

  /**
   * @brief 将一个类注册到ctx中去
   *
   */
  static QJSClassExport Export(QJSModule *module, JSContext *ctx,
                               const std::string &name);

  /**
   * @brief 为这个类导出构造函数
   */
  template <typename... Args>
  QJSClassExport &Construct(const std::string &name);

  template <typename F> QJSClassExport &Method(const std::string &name, F &&f);

  void End();

private:
  class Private;
  std::unique_ptr<Private> d;
};

template <typename T> class QJSClass {
public:
  QJSClass &Method();

  /**
   * @brief 为注册的类添加一个构造函数
   */
  template <typename Signature, typename F>
  static void Constructor(QJSContext *ctx, const std::string &name, F &&f);

private:
  QJSContext *ctx_ = nullptr;
  std::string cls_name_;

public:
  static JSClassID id_;
  static JSClassDef def_;
};

class QJSValue;
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

  QJSValue Value() const;

  /**
   * @brief 调用
   */
  template <typename... Args> QJSValue operator()(Args &&...args);

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

  const JSValue &Raw() const;

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

  QJSModuleExport &operator=(const QJSValue &other);

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

  template <typename T> QJSClassExport<T> Class(const std::string &name);

  QJSModuleExport &operator[](const std::string &name);

  const QJSModuleExport &operator[](const std::string &name) const;

  QJSModuleExport &Export(const std::string &name);

private:
  class Private;
  std::unique_ptr<Private> d;
};

class QJSRuntime {
public:
  QJSRuntime();

  ~QJSRuntime();

  bool IsJobPending() const;

private:
  JSRuntime *rt_ = nullptr;

  friend class QJSContext;
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
