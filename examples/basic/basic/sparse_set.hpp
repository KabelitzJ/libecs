#ifndef BASIC_SPARSE_SET_HPP_
#define BASIC_SPARSE_SET_HPP_

#include <concepts>
#include <utility>
#include <memory>
#include <vector>

#include <libecs/memory.hpp>

namespace basic {

namespace detail {

template<typename Container>
class sparse_set_iterator {

public:

  using value_type = typename Container::value_type;
  using pointer = typename Container::const_pointer;
  using reference = typename Container::const_reference;
  using difference_type = typename Container::difference_type;
  using iterator_category = std::random_access_iterator_tag;

  constexpr sparse_set_iterator() noexcept
  : _container{},
    _offset{} {}

  constexpr sparse_set_iterator(const Container& container, const difference_type offset) noexcept
  : _container{std::addressof(container)},
    _offset{offset} {}

  constexpr ~sparse_set_iterator() noexcept = default;

  constexpr auto operator++() noexcept -> sparse_set_iterator& {
    --_offset;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> sparse_set_iterator {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  constexpr auto operator--() noexcept -> sparse_set_iterator& {
    ++_offset;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> sparse_set_iterator {
    auto copy = *this;
    --(*this);
    return copy;
  }

  constexpr auto operator+=(const difference_type value) noexcept -> sparse_set_iterator& {
    _offset -= value;
    return *this;
  }

  constexpr auto operator+(const difference_type value) const noexcept -> sparse_set_iterator {
    auto copy = *this;
    return (copy += value);
  }

  constexpr auto operator-=(const difference_type value) noexcept -> sparse_set_iterator& {
    _offset += value;
    return *this;
  }

  constexpr auto operator-(const difference_type value) const noexcept -> sparse_set_iterator {
    auto copy = *this;
    return (copy -= value);
  }

  [[nodiscard]] constexpr auto operator[](const difference_type value) const noexcept -> reference {
    return _container->data()[index() - value];
  }

  [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
    return _container->data() + index();
  }

  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    return *operator->();
  }

  [[nodiscard]] constexpr auto data() const noexcept -> pointer {
    return _offset ? _container->data() : nullptr;
  }

  [[nodiscard]] constexpr auto index() const noexcept -> difference_type {
    return _offset - 1;
  }

private:

  const Container* _container;
  difference_type _offset;

}; // class sparse_set_iterator

template<typename Container>
[[nodiscard]] constexpr auto operator==(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept -> bool {
  return lhs.index() == rhs.index();
}

template<typename Container>
[[nodiscard]] constexpr auto operator<=>(const sparse_set_iterator<Container>& lhs, const sparse_set_iterator<Container>& rhs) noexcept -> std::strong_ordering {
  return lhs.index() <=> rhs.index();
}

} // namespace detail

template<std::size_t Value>
[[nodiscard]] inline constexpr auto is_power_of_two() noexcept -> bool {
  return Value && ((Value & (Value - 1)) == 0);
}

template<std::size_t Mod>
requires (is_power_of_two<Mod>())
[[nodiscard]] inline constexpr std::size_t fast_mod(const std::size_t value) noexcept {
  return value & (Mod - 1u);
}

template<std::unsigned_integral Type>
struct sparse_set_traits {
  inline static constexpr auto page_size_v = std::size_t{4096u};
}; // struct sparse_set_traits

template<std::unsigned_integral Type, ecs::allocator_for<Type> Allocator = std::allocator<Type>>
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  using sparse_container_type = std::vector<typename allocator_traits::pointer, ecs::rebound_allocator_t<Allocator, typename allocator_traits::pointer>>;
  using dense_container_type = std::vector<Type, Allocator>;

public:

protected:

  using basic_iterator = detail::sparse_set_iterator<dense_container_type>;

  auto swap_and_pop(const basic_iterator entry) -> void {
    auto* self = _sparse_entry(*entry);

    
  }

private:

  auto _sparse_entry(Type value) -> Type* {
    const auto index = static_cast<std::size_t>(value);
    const auto page = index / sparse_set_traits<Type>::page_size_v;
    return (page < _sparse.size() && _sparse[page]) ? (_sparse[page] + fast_mod<sparse_set_traits<Type>::page_size_v>(index)) : nullptr;
  }

  auto _assure_sparse_page(Type value) -> Type* {
    const auto index = static_cast<std::size_t>(value);
    const auto page = index / sparse_set_traits<Type>::page_size_v;

    if (page >= _sparse.size()) {
      _sparse.resize(page + 1u, nullptr);
    }

    if (!_sparse[page]) {
      auto page_allocator = _dense.get_allocator();
      _sparse[page] = allocator_traits::allocate(page_allocator, sparse_set_traits<Type>::page_size_v);
      std::uninitialized_fill_n(_sparse[page], sparse_set_traits<Type>::page_size_v, std::numeric_limits<Type>::max());
    }

    return _sparse[page] + fast_mod<sparse_set_traits<Type>::page_size_v>(index);
  }

  auto _release_sparse_pages() -> void {
    auto page_allocator = _dense.get_allocator();

    for (auto* page : _sparse) {
      if (page) {
        std::destroy_n(page, sparse_set_traits<Type>::page_size_v);
        allocator_traits::deallocate(page_allocator, page, sparse_set_traits<Type>::page_size_v);
        page = nullptr;
      }
    }
  }

  sparse_container_type _sparse;
  dense_container_type _dense;

}; // class basic_sparse_set

} // namespace basic

#endif // BASIC_SPARSE_SET_HPP_
