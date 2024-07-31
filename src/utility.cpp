// STONKENGINE - Jed Fakhfekh - https://github.com/ougi-washi
#include "utility.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>

std::string se::read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        log_error("Could not read file: %s", path.c_str());
        return ""; // Failed to open file
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

Vector2 operator+(const Vector2& a, const Vector2& b) {
    return {a.x + b.x, a.y + b.y};
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
    return {a.x - b.x, a.y - b.y};
}

Vector2 operator*(const Vector2& a, const Vector2& b) {
    return {a.x * b.x, a.y * b.y};
}

Vector2 operator/(const Vector2& a, const Vector2& b) {
    return {a.x / b.x, a.y / b.y};
}

Vector2 operator*(const Vector2& a, const f32& b) {
    return {a.x * b, a.y * b};
}

Vector2 operator/(const Vector2& a, const f32& b) {
    return {a.x / b, a.y / b};
}

Vector2 operator*(const f32& a, const Vector2& b) {
    return {a * b.x, a * b.y};
}

Vector2 operator/(const f32& a, const Vector2& b) {
    return {a / b.x, a / b.y};
}

Vector2 operator+=(Vector2& a, const Vector2& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

Vector2 operator-=(Vector2& a, const Vector2& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

Vector2 operator*=(Vector2& a, const Vector2& b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}

Vector2 operator/=(Vector2& a, const Vector2& b) {
    a.x /= b.x;
    a.y /= b.y;
    return a;
}

Vector2 operator*=(Vector2& a, const f32& b) {
    a.x *= b;
    a.y *= b;
    return a;
}

Vector2 operator/=(Vector2& a, const f32& b) {
    a.x /= b;
    a.y /= b;
    return a;
}

Vector2 operator*=(const f32& a, Vector2& b) {
    b.x *= a;
    b.y *= a;
    return b;
}

Vector2 operator/=(const f32& a, Vector2& b) {
    b.x /= a;
    b.y /= a;
    return b;
}

b8 operator==(const Vector2& a, const Vector2& b) {
    return (a.x == b.x) && (a.y == b.y);
}

b8 operator!=(const Vector2& a, const Vector2& b) {
    return !(a == b);
}

Vector2 operator-(const Vector2& a) {
    return {-a.x, -a.y};
}

b8 operator>(const Vector2& a, const Vector2& b) {
    return (a.x > b.x) && (a.y > b.y);
}

b8 operator<(const Vector2& a, const Vector2& b) {
    return (a.x < b.x) && (a.y < b.y);
}

b8 operator>=(const Vector2& a, const Vector2& b) {
    return (a.x >= b.x) && (a.y >= b.y);
}

b8 operator<=(const Vector2& a, const Vector2& b) {
    return (a.x <= b.x) && (a.y <= b.y);
}

Vector3 operator+(const Vector3 &a, const Vector3 &b){
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 operator-(const Vector3& a, const Vector3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 operator*(const Vector3& a, const Vector3& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

Vector3 operator/(const Vector3& a, const Vector3& b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

Vector3 operator*(const Vector3& a, const f32& b) {
    return {a.x * b, a.y * b, a.z * b};
}

Vector3 operator/(const Vector3& a, const f32& b) {
    return {a.x / b, a.y / b, a.z / b};
}

Vector3 operator*(const f32& a, const Vector3& b) {
    return {a * b.x, a * b.y, a * b.z};
}

Vector3 operator/(const f32& a, const Vector3& b) {
    return {a / b.x, a / b.y, a / b.z};
}

Vector3 operator+=(Vector3& a, const Vector3& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

Vector3 operator-=(Vector3& a, const Vector3& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

Vector3 operator*=(Vector3& a, const Vector3& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a;
}

Vector3 operator/=(Vector3& a, const Vector3& b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    return a;
}

Vector3 operator*=(Vector3& a, const f32& b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

Vector3 operator/=(Vector3& a, const f32& b) {
    a.x /= b;
    a.y /= b;
    a.z /= b;
    return a;
}

Vector3 operator*=(const f32& a, Vector3& b) {
    b.x *= a;
    b.y *= a;
    b.z *= a;
    return b;
}

Vector3 operator/=(const f32& a, Vector3& b) {
    b.x /= a;
    b.y /= a;
    b.z /= a;
    return b;
}

b8 operator==(const Vector3& a, const Vector3& b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

b8 operator!=(const Vector3& a, const Vector3& b) {
    return !(a == b);
}

Vector3 operator-(const Vector3& a) {
    return {-a.x, -a.y, -a.z};
}

b8 operator>(const Vector3& a, const Vector3& b) {
    return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
}

b8 operator<(const Vector3& a, const Vector3& b) {
    return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
}

b8 operator>=(const Vector3& a, const Vector3& b) {
    return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z);
}

b8 operator<=(const Vector3& a, const Vector3& b) {
    return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
}

Vector4 operator+(const Vector4& a, const Vector4& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Vector4 operator-(const Vector4& a, const Vector4& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

Vector4 operator*(const Vector4& a, const Vector4& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

Vector4 operator/(const Vector4& a, const Vector4& b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

Vector4 operator*(const Vector4& a, const f32& b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

Vector4 operator/(const Vector4& a, const f32& b) {
    return {a.x / b, a.y / b, a.z / b, a.w / b};
}

Vector4 operator*(const f32& a, const Vector4& b) {
    return {a * b.x, a * b.y, a * b.z, a * b.w};
}

Vector4 operator/(const f32& a, const Vector4& b) {
    return {a / b.x, a / b.y, a / b.z, a / b.w};
}

Vector4 operator+=(Vector4& a, const Vector4& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
}

Vector4 operator-=(Vector4& a, const Vector4& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
}

Vector4 operator*=(Vector4& a, const Vector4& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    a.w *= b.w;
    return a;
}

Vector4 operator/=(Vector4& a, const Vector4& b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    a.w /= b.w;
    return a;
}

Vector4 operator*=(Vector4& a, const f32& b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
    return a;
}

Vector4 operator/=(Vector4& a, const f32& b) {
    a.x /= b;
    a.y /= b;
    a.z /= b;
    a.w /= b;
    return a;
}

Vector4 operator*=(const f32& a, Vector4& b) {
    b.x *= a;
    b.y *= a;
    b.z *= a;
    b.w *= a;
    return b;
}

Vector4 operator/=(const f32& a, Vector4& b) {
    b.x /= a;
    b.y /= a;
    b.z /= a;
    b.w /= a;
    return b;
}

b8 operator==(const Vector4& a, const Vector4& b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

b8 operator!=(const Vector4& a, const Vector4& b) {
    return !(a == b);
}

Vector4 operator-(const Vector4& a) {
    return {-a.x, -a.y, -a.z, -a.w};
}

b8 operator>(const Vector4& a, const Vector4& b) {
    return (a.x > b.x) && (a.y > b.y) && (a.z > b.z) && (a.w > b.w);
}

b8 operator<(const Vector4& a, const Vector4& b) {
    return (a.x < b.x) && (a.y < b.y) && (a.z < b.z) && (a.w < b.w);
}

b8 operator>=(const Vector4& a, const Vector4& b) {
    return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z) && (a.w >= b.w);
}

b8 operator<=(const Vector4& a, const Vector4& b) {
    return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z) && (a.w <= b.w);
}
