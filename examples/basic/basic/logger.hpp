#ifndef LIBECS_LOGGER_HPP_
#define LIBECS_LOGGER_HPP_

#include <iostream>

#include <fmt/printf.h>

namespace ecs {

class logger {

public:

  logger() = delete;

  ~logger() = default;

  template<typename... Args>
  static auto info(fmt::format_string<Args...> format, Args&&... args) -> void {
    fmt::print(format, std::forward<Args>(args)...);
  }

};

} // namespace ecs

#endif // LIBECS_LOGGER_HPP_
