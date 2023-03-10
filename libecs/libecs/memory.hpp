#ifndef LIBECS_MEMORY_HPP_
#define LIBECS_MEMORY_HPP_

namespace ecs {


template<typename Allocator, typename Type>
struct rebound_allocator {
  using type = typename std::allocator_traits<Allocator>::rebind_alloc<Type>;
};

template<typename Allocator, typename Type>
using rebound_allocator_t = rebound_allocator<Allocator, Type>::type;

} // namespace ecs

#endif // LIBECS_MEMORY_HPP_
