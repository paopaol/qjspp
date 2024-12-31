#include "benchmark/benchmark.h"
#include "qjs++/QJS++.h"

// --------------------------------------------------------------------
// Benchmark                             Time           CPU Iterations
// --------------------------------------------------------------------
// BM_CallNativeCFunction               73 ns         73 ns    9353562
// BM_CallJSCFunction                  356 ns        352 ns    2290115
// BM_CallJSKambdaFunction             307 ns        307 ns    2272690
// BM_CallNativeCFunctionFloat           0 ns          0 ns 1000000000
// BM_CallJSCFunctionFloat              23 ns         23 ns   30955700
// BM_CallJSKambdaFunctionFloat         24 ns         24 ns   28906210

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
