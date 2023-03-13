#ifndef LIBECS_VIEW_HPP_
#define LIBECS_VIEW_HPP_

#include <algorithm>
#include <tuple>
#include <ranges>
#include <vector>

#include <libecs/memory.hpp>

namespace ecs {

template<typename... Components>
class basic_view {

  template<entity_like Entity, allocator_for<Entity> Allocator>
  friend class basic_registry;

  using underlying_type = std::tuple<const entity&, Components&...>;
  using container_type = std::vector<underlying_type>;

public:

  using value_type = underlying_type;
  using iterator = container_type::iterator;

  ~basic_view() = default;

  [[nodiscard]] auto begin() -> iterator {
    return _values.begin();
  }

  [[nodiscard]] auto end() -> iterator {
    return _values.end();
  }

private:

  basic_view() = default;

  basic_view(container_type values)
  : _values{std::move(values)} { }

  container_type _values;

}; // class basic_view

} // namespace ecs

#endif // LIBECS_VIEW_HPP_
