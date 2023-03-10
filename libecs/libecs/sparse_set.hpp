#ifndef LIBECS_SPARSE_SET_HPP_
#define LIBECS_SPARSE_SET_HPP_

#include <memory>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace ecs {

template<typename Type, typename Allocator = std::allocator<Type>>
class sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;

  template<typename Other>
  using rebound_allocator = typename allocator_traits::rebind_alloc<Other>;

  static_assert(std::is_same_v<typename allocator_traits::value_type, Type>, "Invalid allocator type");

  using dense_storage_type = std::vector<Type, Allocator>;
  using sparse_storage_type = std::unordered_map<Type, std::size_t, std::hash<Type>, std::equal_to<Type>, rebound_allocator<std::pair<const Type, std::size_t>>>;

public:

  using size_type = std::size_t;
  using allocator_type = Allocator;
  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = dense_storage_type::iterator;

  sparse_set() = default;

  sparse_set(const sparse_set& other)
  : _dense{other._dense},
    _sparse{other._sparse} { }

  sparse_set(sparse_set&& other) noexcept
  : _dense{std::move(other._dense)},
    _sparse{std::move(other._sparse)} { }

  virtual ~sparse_set() {
    clear();
  }

  auto operator=(const sparse_set& other) -> sparse_set& {
    if (this != &other) {
      _dense = other._dense;
      _sparse = other._sparse;
    }

    return *this;
  }

  auto operator=(sparse_set&& other) noexcept -> sparse_set& {
    if (this != &other) {
      _dense = std::move(other._dense);
      _sparse = std::move(other._sparse);

      other.clear();
    }

    return *this;
  }

  auto contains(const_reference value) const -> bool {
    if (const auto entry = _sparse.find(value); entry != _sparse.cend()) {
      const auto index = entry->second;

      return index < _dense.size() && _dense.at(index) == value;
    }

    return false;
  }

  auto remove(const_reference value) -> void {
    if (!contains(value)) {
      return;
    }

    _swap_and_pop(value);
  }

  auto clear() -> void {
    _clear();
  }

protected:

  virtual auto _swap_and_pop(const_reference value) -> void {
    const auto index = _sparse.at(value);

    _sparse.at(_dense.back()) = index;
    const auto& old_value = std::exchange(_dense.at(index), _dense.back());

    _dense.pop_back();
    _sparse.erase(old_value);
  }

  virtual auto _clear() -> void {
    _dense.clear();
    _sparse.clear();
  }

  auto _emplace(const_reference value) -> void {
    const auto index = _dense.size();

    _sparse.insert({value, index});
    _dense.push_back(value);
  }

  auto _index(const_reference value) const -> size_type {
    if (const auto entry = _sparse.find(value); entry != _sparse.cend()) {
      return entry->second;
    }

    throw std::out_of_range{"Set does not contain value"};
  }

private:

  dense_storage_type _dense;
  sparse_storage_type _sparse;

}; // class sparse_set

} // namespace ecs

#endif // LIBECS_SPARSE_SET_HPP_
