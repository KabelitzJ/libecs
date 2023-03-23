#ifndef LIBECS_COMPONENT_HANDLE_HPP_
#define LIBECS_COMPONENT_HANDLE_HPP_

#include <utility>

namespace ecs {

template<typename Type>
class component_handle {

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  component_handle() : _ptr{nullptr} { } 

  component_handle(std::nullptr_t) : _ptr{nullptr} { } 

  component_handle(reference value) : _ptr{&value} { } 

  ~component_handle() = default;

  auto operator->() -> pointer {
    return _ptr;
  }

  auto operator*() -> reference {
    return *_ptr;
  }

  operator bool() const {
    return _ptr != nullptr;
  }

private:

  pointer _ptr;

}; // class component_handle

} // namespace ecs

#endif // LIBECS_COMPONENT_HANDLE_HPP_
