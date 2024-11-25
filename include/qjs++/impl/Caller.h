#pragma once

#include <absl/utility/utility.h>

#include <tuple>

#include "traits/JSValueTraits.h"

namespace qjs {

/**
 * @brief decode JSValues to tuple
 */
template <typename Tuple, std::size_t... Is>
Tuple JSArrayUnwrapImpl(JSContext *ctx, int argc, JSValueConst *argv,
                        absl::index_sequence<Is...>) {
  return std::make_tuple(
      ValueTraits<typename std::tuple_element<Is, Tuple>::type>::Unwrap(
          ctx, argv[Is])...);
}

template <typename... Args>
std::tuple<Args...> JSArrayUnwrap(JSContext *ctx, int argc,
                                  JSValueConst *argv) {
  return JSArrayUnwrapImpl<std::tuple<Args...>>(
      ctx, argc, argv, absl::index_sequence_for<Args...>());
}

/**
 * @brief 调用c++原生接口，接口参数从JS中获取
 */
template <typename R, typename... Args, typename Callable>
R InvokeNative(JSContext *ctx, Callable &&call, int argc, JSValueConst *argv) {
  auto args =
      JSArrayUnwrap<typename std::decay<Args>::type...>(ctx, argc, argv);
  return absl::apply(std::forward<Callable>(call), std::move(args));
}
} // namespace qjs
