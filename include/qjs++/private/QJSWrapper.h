#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "quickjs/quickjs.h"

class QJSContext;
class QJSModule;
class QJSValue;

template <typename T> class QJSClassExportor {
public:
  QJSClassExportor();

  QJSClassExportor(JSContext *ctx, QJSModule *module, JSValue proto);

  ~QJSClassExportor();

  QJSClassExportor(QJSClassExportor &&other);

  QJSClassExportor &operator=(QJSClassExportor &&other);

  /**
   * @brief 将一个类注册到ctx中去
   *
   * @module 这个类注册到的module
   *
   * @ctx 这个类要注册到的JSContext
   *
   * @name 为这个类指定一个类名
   */
  static QJSClassExportor New(QJSModule *module, JSContext *ctx,
                              const std::string &name);

  /**
   * @brief 为这个类注册构造函数
   *
   * @name 构造函数名
   */
  template <typename... Args>
  QJSClassExportor &Construct(const std::string &name);

  /**
   * @brief 为这个类导出方法
   */
  template <typename F>
  QJSClassExportor &Method(const std::string &name, F &&f);

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

/**
 * @brief module属性辅助类
 */
class QJSModuleProperty {
public:
  /**
   * @brief 向ctx注册一个module，
   *
   * @name 要注册的module名称
   */
  QJSModuleProperty(JSContext *ctx, JSModuleDef *m, const std::string &name);

  ~QJSModuleProperty();

  QJSModuleProperty(QJSModuleProperty &&other);

  QJSModuleProperty &operator=(QJSModuleProperty &&other);

  QJSModuleProperty &operator=(const QJSValue &other);

  QJSModuleProperty(const QJSModuleProperty &other) = delete;

  QJSModuleProperty &operator=(const QJSModuleProperty &other) = delete;

  /**
   * @brief 为该属性设置一个类型为F的f值
   *
   * 即，绑定一个函数f到该属性
   */
  template <typename Signature, typename F> QJSModuleProperty &Function(F &&f);

  /**
   * @brief 假如该属性是一个函数的话，那么这个可以调用之前绑定的函数
   *
   * @return 返回之前绑定的函数运行的返回值
   */
  template <typename... Args> QJSValue operator()(Args &&...args);

private:
  void FreeInternal();

  void StealFrom(QJSModuleProperty &other);

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

  template <typename T> QJSClassExportor<T> Class(const std::string &name);

  QJSModuleProperty &operator[](const std::string &name);

  const QJSModuleProperty &operator[](const std::string &name) const;

  /**
   * @brief 导出一个module
   *
   * @name 要导出的module名称
   */
  QJSModuleProperty &Export(const std::string &name);

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
