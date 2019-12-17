#pragma once
#include <algorithm>

enum class CollisionSide : unsigned char
{
  NONE = 0x00,
  UP = 0x01,
  DOWN = 0x02,
  RIGHT = 0x04,
  LEFT = 0x08
};

//______________________________________________________________________________
void operator|=(CollisionSide& the, CollisionSide other);
//______________________________________________________________________________
void operator&=(CollisionSide& the, CollisionSide other);
//______________________________________________________________________________
CollisionSide operator&(CollisionSide a, CollisionSide b);
//______________________________________________________________________________
CollisionSide operator~(CollisionSide og);
//______________________________________________________________________________
static bool HasState(const CollisionSide& state, CollisionSide other) { return (state & other) == other; }


//______________________________________________________________________________
template <typename T>
class Vector2
{
public:
  Vector2<T>() : x(0), y(0) {}
  Vector2<T>(T x, T y) : x(x), y(y) {}

  T x;
  T y;

  void operator+=(const Vector2<T>& other) { x += other.x; y += other.y; }
  void operator*=(const Vector2<T>& other) { x *= other.x; y *= other.y; }

  Vector2<T> Unit();
  T Dot(const Vector2<T>& other);
  T Magnitude();
};

template <typename T> Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs);
template <typename T> Vector2<T> operator*(const Vector2<T>& lhs, T rhs);
template <typename T> Vector2<T> operator*(T lhs, const Vector2<T>& rhs);
template <typename T, typename U> Vector2<T> operator*(const Vector2<T>& lhs, U rhs);

//______________________________________________________________________________
template <typename T>
struct OverlapInfo
{
  Vector2<T> amount;
  CollisionSide collisionSides;
  int numCollisionSides;

  //!
  OverlapInfo() : amount(0.0, 0.0), collisionSides(CollisionSide::NONE), numCollisionSides(0) {}
};

//______________________________________________________________________________
template <typename T>
class Rect
{
public:
  Rect() : _beg(0, 0), _end(0, 0) {}
  Rect(Vector2<T> beg, Vector2<T> end) : _beg(beg), _end(end) {}
  Rect(T xMin, T yMin, T xMax, T yMax) : _beg(xMin, yMin), _end(xMax, yMax) {}

  const T Width() { return _end.x - _beg.x; }
  const T Height() { return _end.y - _beg.y; }

  void Move(const Vector2<T>& vec);

  //template <typename U>
  //Vector2<T> Overlap(const U& other, Vector2<T> incidentVector);

  //template <typename U>
  //bool Collides(const U& other);

  OverlapInfo<T> Overlap(const Rect<T>& other);
  bool Collides(const Rect<T>& other);
  bool Collides(const Vector2<T>& other);

  Rect<T> GetIntersectionRect(const Rect<T>& other);

  const Vector2<T>& Beg() const { return _beg; }
  const Vector2<T>& End() const { return _end; }

private:
  Vector2<T> _beg;
  Vector2<T> _end;

};

//______________________________________________________________________________
template <typename T>
inline Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) { return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y); }

//______________________________________________________________________________
template <typename T>
inline Vector2<T> operator*(const Vector2<T>& lhs, T rhs) { return Vector2<T>(lhs.x * rhs, lhs.y * rhs); }

//______________________________________________________________________________
template <typename T>
inline Vector2<T> operator*(T lhs, const Vector2<T>& rhs) { return Vector2<T>(lhs * rhs.x, lhs * rhs.y); }

//______________________________________________________________________________
template <typename T, typename U>
inline Vector2<T> operator*(const Vector2<T>& lhs, U rhs) { return Vector2<T>(static_cast<T>(static_cast<U>(lhs.x) * rhs), static_cast<T>(static_cast<U>(lhs.y) * rhs)); }
