#ifndef LIBECS_ENTITY_HPP_
#define LIBECS_ENTITY_HPP_

#include <cinttypes>
#include <memory>
#include <type_traits>

namespace ecs {

template<typename Type>
struct basic_entity_traits;

template<>
struct basic_entity_traits<std::uint32_t> {
  using value_type = std::uint32_t;

  using id_type = std::uint32_t;
  using version_type = std::uint16_t;

  inline static constexpr auto entity_mask_v = id_type{0xFFFFF};
  inline static constexpr auto version_mask_v = id_type{0xFFF};
};

template<>
struct basic_entity_traits<std::uint64_t> {
  using value_type = std::uint64_t;

  using id_type = std::uint64_t;
  using version_type = std::uint32_t;

  inline static constexpr auto entity_mask_v = id_type{0xFFFFFFFF};
  inline static constexpr auto version_mask_v = id_type{0xFFFFFFFF};
};

template<typename Type>
requires (std::is_enum_v<Type>)
struct basic_entity_traits<Type> : basic_entity_traits<std::underlying_type_t<Type>> {
  using value_type = Type;
};

template<typename Type>
requires (std::is_class_v<Type>)
struct basic_entity_traits<Type> : basic_entity_traits<typename Type::entity_type> {
  using value_type = Type;
};

template<typename Type>
struct entity_traits : basic_entity_traits<Type> {
  using value_type = basic_entity_traits<Type>::value_type;
  using id_type = basic_entity_traits<Type>::id_type;
  using version_type = basic_entity_traits<Type>::version_type;

  inline static constexpr auto entity_mask_v = basic_entity_traits<Type>::entity_mask_v;
  inline static constexpr auto version_mask_v = basic_entity_traits<Type>::version_mask_v;
  inline static constexpr auto version_shift_v = std::popcount(entity_mask_v);

  static constexpr auto to_underlying(const value_type value) noexcept -> id_type {
    return static_cast<id_type>(value);
  }

  static constexpr auto to_id(const value_type value) noexcept -> id_type {
    return (to_underlying(value) & entity_mask_v);
  }

  static constexpr auto to_version(const value_type value) noexcept -> version_type {
    return (to_underlying(value) >> version_shift_v);
  }

  static constexpr auto construct(const id_type id, const version_type version) noexcept -> value_type {
    return value_type{(id & entity_mask_v) | (static_cast<id_type>(version) << version_shift_v)};
  }

  static constexpr auto next(const value_type value) noexcept -> value_type {
    const auto version = to_version(value) + 1;
    return construct(to_id(value), static_cast<version_type>(version + (version == version_mask_v)));
  }
};

template<typename Type, typename Traits = entity_traits<Type>>
class basic_entity {

  template<typename, typename>
  friend class basic_registry;

  using traits = Traits;

public:

  using value_type = traits::value_type;
  using id_type = traits::id_type;
  using version_type = traits::version_type;

  constexpr basic_entity(const basic_entity&) noexcept = default;

  constexpr basic_entity(basic_entity&&) noexcept = default;

  constexpr ~basic_entity() noexcept = default;

  constexpr auto operator=(const basic_entity&) noexcept -> basic_entity&;

  constexpr auto operator=(basic_entity&&) noexcept -> basic_entity&;

  constexpr auto operator==(const basic_entity& other) noexcept -> bool {
    return traits::to_underlying(_value) == traits::to_underlying(other._value);
  }

private:

  constexpr basic_entity(const id_type id, const version_type version) noexcept
  : _value{traits::construct(id, version)} { }

  constexpr auto _id() const noexcept -> id_type {
    return traits::to_id(_value);
  }

  constexpr auto _version() const noexcept -> version_type {
    return traits::to_version(_value);
  }

  constexpr auto _increment_version() noexcept {
    _value = traits::next(_value);
  }

  value_type _value{};

}; // class basic_entity


namespace detail {

enum class entity_tag : std::uint32_t {};

} // namespace detail

using entity = basic_entity<detail::entity_tag>;

} // namespace ecs

#endif // LIBECS_ENTITY_HPP_
