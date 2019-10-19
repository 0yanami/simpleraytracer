#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

class vec3
{
public:
    vec3() {}
    vec3(float e0, float e1, float e2)
    {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }
    
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const vec3 &operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float &operator[](int i) { return e[i]; }

    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

    float e[3];

    friend std::istream &operator>>(std::istream &is, vec3 &t);

    friend std::ostream &operator<<(std::ostream &os, const vec3 &t);

    inline vec3 operator*(float t) const { return vec3(e[0] * t, e[1] * t, e[2] * t); }
    inline vec3 operator/(float t) const { return vec3(e[0] / t, e[1] / t, e[2] / t); }
    inline vec3 unit_vector() const { return *this / (float)length(); }

    inline void make_unit_vector()
    {
        float k = static_cast<float>(1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]));
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
    }

    inline vec3 &operator+=(const vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    inline vec3 &operator*=(const vec3 &v)
    {
        e[0] *= v.e[0];
        e[1] *= v.e[1];
        e[2] *= v.e[2];
        return *this;
    }

    inline vec3 &operator/=(const vec3 &v)
    {
        e[0] /= v.e[0];
        e[1] /= v.e[1];
        e[2] /= v.e[2];
        return *this;
    }

    inline vec3 &operator-=(const vec3 &v)
    {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }

    inline vec3 &operator*=(const float t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    inline vec3 &operator/=(const float t)
    {
        float k = static_cast<float>(1.0 / t);
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
        return *this;
    }

    //op simples
    inline vec3 operator+(const vec3 &v2) const
    {
        return vec3(e[0] + v2.e[0], e[1] + v2.e[1], e[2] + v2.e[2]);
    }
    inline vec3 operator-(const vec3 &v2) const
    {
        return vec3(e[0] - v2.e[0], e[1] - v2.e[1], e[2] - v2.e[2]);
    }
    inline vec3 operator*(const vec3 &v2) const
    {
        return vec3(e[0] * v2.e[0], e[1] * v2.e[1], e[2] * v2.e[2]);
    }
    inline vec3 operator/(const vec3 &v2) const
    {
        return vec3(e[0] / v2.e[0], e[1] / v2.e[1], e[2] / v2.e[2]);
    }

    inline float dot(const vec3 &v2) const
    {
        return e[0] * v2.e[0] + e[1] * v2.e[1] + e[2] * v2.e[2];
    }
    inline vec3 cross(const vec3 &v2)
    {
        return vec3(
            (e[1] * v2.e[2] - e[2] * v2.e[1]),
            (-(e[0] * v2.e[2] - e[2] * v2.e[0])),
            (e[0] * v2.e[1] - e[1] * v2.e[0]));
    }
};

std::istream &operator>>(std::istream &is, vec3 &t)
{
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

std::ostream &operator<<(std::ostream &os, const vec3 &t)
{
    os << t.e[0] << " " << t.e[1] << " " << t.e[2];
    return os;
}

inline vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - n * v.dot(n) * 2;
}

