#ifndef LIBECS_MEMORY_HPP_
#define LIBECS_MEMORY_HPP_

#include <memory>
#include <type_traits>

namespace ecs {

template<typename Allocator, typename Type>
concept allocator_for = std::is_same_v<typename std::allocator_traits<Allocator>::value_type, Type>;

template<typename Allocator, typename Type>
struct rebound_allocator {
  using type = typename std::allocator_traits<Allocator>::rebind_alloc<Type>;
};

template<typename Allocator, typename Type>
using rebound_allocator_t = rebound_allocator<Allocator, Type>::type;

} // namespace ecs

#endif // LIBECS_MEMORY_HPP_
