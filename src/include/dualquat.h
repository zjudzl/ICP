// Copyright 2017 John Stechschulte
#ifndef SRC_INCLUDE_DUALQUAT_H_
#define SRC_INCLUDE_DUALQUAT_H_
#include <Eigen/Dense>
#include <pcl/point_types.h>
#include <iostream>
#include <cmath>

template <typename T>
class Quat {
 public:
    union {
      T vector[4];
      struct {
        T x; T y; T z; T w;
      };
    };

    Quat() : x((T) 0.), y((T) 0.), z((T) 0.), w((T) 0.) {}
    Quat(T w, T x, T y, T z) :
      x(x), y(y), z(z), w(w) {}
    explicit Quat(T vector[4]) :
      x(vector[0]), y(vector[1]), z(vector[2]), w(vector[3]) {}
    explicit Quat(pcl::PointXYZ p);
    Quat(Eigen::Vector3d vector, T rotation_radians);
    explicit Quat(Eigen::Matrix<T, 4, 1> vector);
    static Quat fromRotationMatrix(Eigen::Matrix<T, 3, 3> m);

    Quat conjugate() const;
    T magnitude() const;
    void normalize();
    Eigen::Matrix<T, 3, 3> K() const;
    Eigen::Matrix<T, 4, 4> Q() const;
    Eigen::Matrix<T, 4, 4> W() const;
    Eigen::Matrix<T, 4, 4> R() const;
    Eigen::Matrix<T, 3, 3> Rot() const;
    Quat& Identity();
    T Angle() const;
    Eigen::Vector3d RotAxis() const;

    Quat& operator+=(const Quat& rhs) {
      this->w += rhs.w; this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
      return *this;
    }

    friend Quat operator+(Quat lhs, const Quat& rhs) {
      lhs += rhs;
      return lhs;
    }

    Quat operator-() const {
      return Quat(-this->w, -this->x, -this->y, -this->z);
    }

    Quat& operator-=(const Quat& rhs) {
      *this += -rhs;
      return *this;
    }

    friend Quat operator-(Quat lhs, const Quat& rhs) {
      lhs -= rhs;
      return lhs;
    }

    Quat& operator*=(const Quat& rhs) {
      T w = this->w*rhs.w - this->x*rhs.x - this->y*rhs.y - this->z*rhs.z;
      T x = this->x*rhs.w + this->w*rhs.x - this->z*rhs.y + this->y*rhs.z;
      T y = this->y*rhs.w + this->z*rhs.x + this->w*rhs.y - this->x*rhs.z;
      T z = this->z*rhs.w - this->y*rhs.x + this->x*rhs.y + this->w*rhs.z;
      this->w = w; this->x = x; this->y = y; this->z = z;
      return *this;
    }

    Quat& operator*=(const T& scalar) {
      this->w *= scalar;
      this->x *= scalar;
      this->y *= scalar;
      this->z *= scalar;
      return *this;
    }

    friend Quat operator*(Quat lhs, const Quat& rhs) {
      lhs *= rhs;
      return lhs;
    }

    Quat& operator/=(const Quat& rhs) {
      T denom = rhs.magnitude();
      denom *= denom;
      T w = (this->w*rhs.w + this->x*rhs.x + this->y*rhs.y + this->z*rhs.z)/denom;
      T x = (this->x*rhs.w - this->w*rhs.x - this->z*rhs.y + this->y*rhs.z)/denom;
      T y = (this->y*rhs.w + this->z*rhs.x - this->w*rhs.y - this->x*rhs.z)/denom;
      T z = (this->z*rhs.w - this->y*rhs.x + this->x*rhs.y - this->w*rhs.z)/denom;
      this->w = w; this->x = x; this->y = y; this->z = z;
      return *this;
    }

    friend Quat operator/(Quat lhs, const Quat& rhs) {
      lhs /= rhs;
      return lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Quat& rhs) {
      return os << rhs.w << " + " << rhs.x << "i + " << rhs.y << "j + " <<
        rhs.z << "k";
    }
};

template <typename T>
inline bool operator==(const Quat<T>& lhs, const Quat<T>& rhs) {
  return (lhs.w == rhs.w) && (lhs.x == rhs.x) &&
    (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

template <typename T>
inline bool operator!=(const Quat<T>& lhs, const Quat<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
inline Quat<T> operator*(T const& scalar, Quat<T> rhs) {
  return rhs *= scalar;
}

template <typename T>
class DualQuat {
 public:
    Quat<T> r;
    Quat<T> d;

    DualQuat() : r(Quat<T>()), d(Quat<T>()) {}
    DualQuat(Quat<T> r, Quat<T> d) : r(r), d(d) {}
    explicit DualQuat(pcl::PointXYZ point);

    DualQuat conjugate() const;
    T realMagnitude() const;
    T sumSq() const;
    void normalize();
    Eigen::Matrix<T, 4, 4> Matrix() const;
    DualQuat& Identity();
    Eigen::Vector3d getTranslation() const;

    DualQuat& operator+=(const DualQuat& rhs) {
      this->r += rhs.r;
      this->d += rhs.d;
      return *this;
    }

    friend DualQuat operator+(DualQuat lhs, const DualQuat& rhs) {
      lhs += rhs;
      return lhs;
    }

    DualQuat operator-() const {
      return DualQuat(-r, -d);
    }

    DualQuat& operator-=(const DualQuat& rhs) {
      *this += -rhs;
      return *this;
    }

    friend DualQuat operator-(DualQuat lhs, const DualQuat& rhs) {
      lhs -= rhs;
      return lhs;
    }

    DualQuat& operator*=(const DualQuat& rhs) {
      Quat<T> r = this->r * rhs.r;
      this->d = this->r * rhs.d + this->d * rhs.r;
      this->r = r;
      return *this;
    }

    DualQuat& operator*=(const T& scalar) {
      this->r *= scalar;
      this->d *= scalar;
      return *this;
    }

    friend DualQuat operator*(DualQuat lhs, const DualQuat& rhs) {
      lhs *= rhs;
      return lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const DualQuat& rhs) {
      return os << "(" << rhs.r << ") + (" << rhs.d << ")e";
    }
};

template <typename T>
DualQuat<T>& DualQuatIdentity();

template <typename T>
inline bool operator==(const DualQuat<T>& lhs, const DualQuat<T>& rhs) {
  return (lhs.r == rhs.r) && (lhs.d == rhs.d);
}

template <typename T>
inline bool operator!=(const DualQuat<T>& lhs, const DualQuat<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
inline DualQuat<T> operator*(T const& scalar, DualQuat<T> rhs) {
  return rhs *= scalar;
}

#endif  // SRC_INCLUDE_DUALQUAT_H_
