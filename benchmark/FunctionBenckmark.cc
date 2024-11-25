#include "benchmark/benchmark.h"
#include "qjs++/QJS++.h"

static std::string c_string_function(const std::string &value) { return value; }

static float c_float_function(float value) { return value; }

static void BM_CallNativeCFunction(benchmark::State &state) {
  for (auto _ : state) {
    c_string_function("12333333333333333333333333333333333");
  }
}

BENCHMARK(BM_CallNativeCFunction);

static void BM_CallJSCFunction(benchmark::State &state) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  qjs::Value v(&ctx);

  v = c_string_function;

  for (auto _ : state) {
    v("12333333333333333333333333333333333");
  }
}

BENCHMARK(BM_CallJSCFunction);

static void BM_CallJSKambdaFunction(benchmark::State &state) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  qjs::Value v(&ctx);

  v.SetLambda<std::string(const std::string &)>(
      [](const std::string &value) { return value; });

  for (auto _ : state) {
    v("12333333333333333333333333333333333");
  }
}

BENCHMARK(BM_CallJSKambdaFunction);

static void BM_CallNativeCFunctionFloat(benchmark::State &state) {
  for (auto _ : state) {
    c_float_function(123.f);
  }
}

BENCHMARK(BM_CallNativeCFunctionFloat);

static void BM_CallJSCFunctionFloat(benchmark::State &state) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  qjs::Value v(&ctx);

  v = c_float_function;

  for (auto _ : state) {
    v(123.f);
  }
}

BENCHMARK(BM_CallJSCFunctionFloat);

static void BM_CallJSKambdaFunctionFloat(benchmark::State &state) {
  qjs::Runtime rt;
  qjs::Context ctx(rt);

  qjs::Value v(&ctx);

  v.SetLambda<float(float)>([](float value) { return value; });

  for (auto _ : state) {
    v(123.f);
  }
}

BENCHMARK(BM_CallJSKambdaFunctionFloat);
