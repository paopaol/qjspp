#pragma once

#include "QJSValueTraits.h"
#include <absl/utility/utility.h>
#include <tuple>

/**
 * @brief 解码quickjs数组到c++tuple中
 */
template <typename Tuple, std::size_t... Is>
Tuple JSArrayUnwrapImpl(JSContext *ctx, int argc, JSValueConst *argv,
                        absl::index_sequence<Is...>) {
  return Tuple(
      QJSValueTraits<typename std::tuple_element<Is, Tuple>::type>::Unwrap(
          ctx, argv[Is])...);
}

template <typename... Args>
std::tuple<Args...> JSArrayUnwrap(JSContext *ctx, int argc,
                                  JSValueConst *argv) {
  using Tuple = std::tuple<typename std::decay<Args>::type...>;
  return JSArrayUnwrapImpl<Tuple>(ctx, argc, argv,
                                  absl::index_sequence_for<Args...>());
}

/**
 * @brief 调用c++原生接口，接口参数从JS中获取
 */
template <typename R, typename... Args, typename Callable>
R InvokeNative(JSContext *ctx, Callable &&call, int argc, JSValueConst *argv) {
  return absl::apply(std::forward<Callable>(call),
                     JSArrayUnwrap<Args...>(ctx, argc, argv));
}
