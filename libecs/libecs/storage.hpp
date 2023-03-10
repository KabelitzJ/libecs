#ifndef LIBECS_STORAGE_HPP_
#define LIBECS_STORAGE_HPP_

#include <type_traits>
#include <iostream>
#include <limits>
#include <vector>

#include <libecs/sparse_set.hpp>
#include <libecs/memory.hpp>

namespace ecs {

template<typename Key, typename Value, typename Allocator = std::allocator<Value>>
class storage : public sparse_set<Key, typename std::allocator_traits<Allocator>::rebind_alloc<Key>> {

  using allocator_traits = std::allocator_traits<Allocator>;

  static_assert(std::is_same_v<typename allocator_traits::value_type, Value>, "Invalid allocator type");

  using base_type = sparse_set<Key, rebound_allocator_t<Allocator, Key>>;

  using container_type = std::vector<Value, Allocator>;

public:

  using key_type = Key;
  using value_type = Value;
  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  storage()
  : base_type{} { }

  storage(const storage& other) = delete;

  storage(storage&& other) noexcept
  : base_type{std::move(other)},
    _container{std::move(other._container)} { }

  ~storage() {
    base_type::clear();
  }

  auto operator=(const storage& other) -> storage& = delete;

  auto operator=(storage&& other) noexcept -> storage& {
    if (this != &other) {
      base_type::clear();

      base_type::operator=(std::move(other));
      _container = std::move(other._container);

      other.clear();
    }

    return *this;
  }

  template<typename... Args>
  requires(std::constructible_from<Value, Args...>)
  auto add(const key_type& key, Args&&... args) -> reference {
    if (auto entry = find(key); entry != end()) {
      return (*entry = value_type{std::forward<Args>(args)...});
    }

    base_type::_emplace(key);

    _container.emplace_back(std::forward<Args>(args)...);

    return _container.back();
  }

  auto begin() -> iterator {
    return _container.begin();
  }

  auto begin() const -> const_iterator {
    return _container.begin();
  }

  auto cbegin() const -> const_iterator {
    return _container.cbegin();
  }

  auto end() -> iterator {
    return _container.end();
  }

  auto end() const -> const_iterator {
    return _container.end();
  }

  auto cend() const -> const_iterator {
    return _container.cend();
  }

  auto find(const key_type& key) -> iterator {
    if (base_type::contains(key)) {
      auto entry = begin();
      std::advance(entry, base_type::_index(key));
      return entry;
    }

    return end();
  }

  auto find(const key_type& key) const -> const_iterator {
    if (base_type::contains(key)) {
      auto entry = cbegin();
      std::advance(entry, base_type::_index(key));
      return entry;
    }

    return cend();
  }

protected:

  auto _swap_and_pop(const key_type& key) -> void override {
    const auto index = base_type::_index(key);

    using std::swap;
    swap(_container.at(index), _container.back());

    _container.pop_back();

    base_type::_swap_and_pop(key);
  }

  auto _clear() -> void override {
    base_type::_clear();
    _container.clear();
  }

private:

  container_type _container;

}; // class storage

} // namespace ecs

#endif // LIBECS_STORAGE_HPP_
