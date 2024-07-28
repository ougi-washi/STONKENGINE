// Abstract City - Jed Fakhfekh - https://github.com/ougi-washi
#pragma once
#include "types.h"
#include "raylib.h"
#include <string>

namespace ac{
    std::string read_file(const std::string& path);
} // namespace ac

// Vector2
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Vector2& b);
Vector2 operator/(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const f32& b);
Vector2 operator/(const Vector2& a, const f32& b);
Vector2 operator*(const f32& a, const Vector2& b);
Vector2 operator/(const f32& a, const Vector2& b);
Vector2 operator+=(Vector2& a, const Vector2& b);
Vector2 operator-=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, const Vector2& b);
Vector2 operator/=(Vector2& a, const Vector2& b);
Vector2 operator*=(Vector2& a, const f32& b);
Vector2 operator/=(Vector2& a, const f32& b);
Vector2 operator*=(const f32& a, Vector2& b);
Vector2 operator/=(const f32& a, Vector2& b);
b8 operator==(const Vector2& a, const Vector2& b);
b8 operator!=(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a);
b8 operator>(const Vector2& a, const Vector2& b);
b8 operator<(const Vector2& a, const Vector2& b);
b8 operator>=(const Vector2& a, const Vector2& b);
b8 operator<=(const Vector2& a, const Vector2& b);
// Vector3
Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& a, const f32& b);
Vector3 operator/(const Vector3& a, const f32& b);
Vector3 operator*(const f32& a, const Vector3& b);
Vector3 operator/(const f32& a, const Vector3& b);
Vector3 operator+=(Vector3& a, const Vector3& b);
Vector3 operator-=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, const Vector3& b);
Vector3 operator/=(Vector3& a, const Vector3& b);
Vector3 operator*=(Vector3& a, const f32& b);
Vector3 operator/=(Vector3& a, const f32& b);
Vector3 operator*=(const f32& a, Vector3& b);
Vector3 operator/=(const f32& a, Vector3& b);
b8 operator==(const Vector3& a, const Vector3& b);
b8 operator!=(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a);
b8 operator>(const Vector3& a, const Vector3& b);
b8 operator<(const Vector3& a, const Vector3& b);
b8 operator>=(const Vector3& a, const Vector3& b);
b8 operator<=(const Vector3& a, const Vector3& b);
// Vector4
Vector4 operator+(const Vector4& a, const Vector4& b);
Vector4 operator-(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, const Vector4& b);
Vector4 operator/(const Vector4& a, const Vector4& b);
Vector4 operator*(const Vector4& a, const f32& b);
Vector4 operator/(const Vector4& a, const f32& b);
Vector4 operator*(const f32& a, const Vector4& b);
Vector4 operator/(const f32& a, const Vector4& b);
Vector4 operator+=(Vector4& a, const Vector4& b);
Vector4 operator-=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, const Vector4& b);
Vector4 operator/=(Vector4& a, const Vector4& b);
Vector4 operator*=(Vector4& a, const f32& b);
Vector4 operator/=(Vector4& a, const f32& b);
Vector4 operator*=(const f32& a, Vector4& b);
Vector4 operator/=(const f32& a, Vector4& b);
b8 operator==(const Vector4& a, const Vector4& b);
b8 operator!=(const Vector4& a, const Vector4& b);
Vector4 operator-(const Vector4& a);
b8 operator>(const Vector4& a, const Vector4& b);
b8 operator<(const Vector4& a, const Vector4& b);
b8 operator>=(const Vector4& a, const Vector4& b);
b8 operator<=(const Vector4& a, const Vector4& b);
