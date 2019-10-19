#ifndef CAMERAH
#define CAMERAH
#include <cmath>
#include <random>
#include "ray.h"


class camera
{
public:
    vec3 random_in_unit_disk(){
        vec3 p;
        do {
            p = vec3(dis(gen),dis(gen),0)*2 - vec3(1,1,0);
        } while (p.dot(p) >= 1.0);
        return p;
    }


    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect,float aperture, float focus_dist, float t0, float t1){ //vfov in degrees
        time0 = t0;
        time1 = t1;
        lens_radius = aperture / 2;
        float theta = vfov*(3.141/180);
        float half_height = tan(theta/2);
        float half_width = aspect *half_height;
        origin = lookfrom;
        w = (lookfrom - lookat).unit_vector();
        u = (vup.cross(w)).unit_vector();
        v = w.cross(u);
        
        lower_left_corner = origin - u*half_width*focus_dist - v*half_height*focus_dist - w*focus_dist;
        horizontal = u*half_width*focus_dist*2;
        vertical = v*half_height*focus_dist*2;
    }
    ray get_ray(float s, float t){
        vec3 rd = random_in_unit_disk()*lens_radius;
        vec3 offset = u * rd.x() + v * rd.y();
        float time = time0 + dis(gen)*(time1-time0);
        return ray(origin + offset, lower_left_corner+ horizontal*s+ vertical*t - origin - offset, time); }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float time0, time1;
    float lens_radius;
    

    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_real_distribution<> dis;
};


std::mt19937 camera::gen = std::mt19937(rd());
std::uniform_real_distribution<> camera::dis = std::uniform_real_distribution<>(0, 1);

#endif
                                