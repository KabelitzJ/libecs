#ifndef LIBECS_VECTOR3_HPP_
#define LIBECS_VECTOR3_HPP_

#include <cmath>

namespace ecs {

class vector3 {

public:

  std::float_t x{};
  std::float_t y{};
  std::float_t z{};

  vector3() = default;

  vector3(std::float_t _x, std::float_t _y, std::float_t _z)
  : x{_x}, y{_y}, z{_z} { }

  ~vector3() = default;

  // auto operator+=(const vector3& other) -> vector3& {
  //   x += other.x;
  //   y += other.y;
  //   z += other.z;

  //   return *this;
  // }

  // auto operator+=(std::float_t other) -> vector3& {
  //   x += other;
  //   y += other;
  //   z += other;

  //   return *this;
  // }

}; // class vector3

// auto operator+(vector3 lhs, const vector3& rhs) -> vector3 {
//   return lhs += rhs;
// }

// auto operator+(vector3 lhs, std::float_t rhs) -> vector3 {
//   return lhs += rhs;
// }

} // namespace ecs

#endif // LIBECS_VECTOR3_HPP_
