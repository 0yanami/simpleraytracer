#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray
{
public:
    ray() = default;
    ray(const vec3 &a, const vec3 &b, float ti = 0.0){
        A = a;
        B = b;
        _time = ti;
    }
    const vec3& origin() const { return A; }
    const vec3& direction() const { return B; }
    float time() const { return _time;}
    vec3 point_at_parameter(float t) const{
        vec3 bt = B * t;
        return A + bt;
    }
    vec3 A;
    vec3 B;
    float _time;
};

#endif