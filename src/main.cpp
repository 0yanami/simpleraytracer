#ifdef __clang__
#define STBIWDEF static inline
#endif
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <random>
#include <memory>
#include <omp.h>
#include <cstdlib>

#include "moving_sphere.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "bvh.h"
#include "texture.h"
#include "perlin.h"
#include "image_texture.h"
#include "rect.h"
#include "flip_normal.h"

using namespace std;

vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, 999999.9, rec))
    {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return emitted + attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return emitted;
        }
    }
    else
    {
        vec3 unit_direction = r.direction().unit_vector();
        float t = 0.5 * (unit_direction.y() + 1.0);
        return vec3(0.5, 0.5, 0.5);
    }
}

void render_frame(camera cam, vec3 res, hitable *world)
{
    //Anti Aliasing rng generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);

    int nx = int(res.x());
    int ny = int(res.y());
    int ns = int(res.z());

    auto image_buffer = (unsigned char *)malloc(nx * ny * 3 * sizeof(unsigned char));

    //init camera



#pragma omp parallel for
    for (int i = 0; i < ny * nx; i++){
        vec3 col(0, 0, 0);
        for (int s = 0; s < ns; s++){
            float u = float(i % int(nx) + dis(gen)) / float(nx);
            float v = 1.0 - float(int(i / nx) + 1 + dis(gen)) / float(ny);
            ray r = cam.get_ray(u, v);
            vec3 p = r.point_at_parameter(2.0);
            col += color(r, world, 0);
        }
        col /= float(ns);
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
        image_buffer[i * 3 + 0] = int(255.99 * col[0]);
        image_buffer[i * 3 + 1] = int(255.99 * col[1]);
        image_buffer[i * 3 + 2] = int(255.99 * col[2]);
    }
    //file path
    ostringstream fileNameStream("out");
    fileNameStream << "output/out.png";
    string fileName = fileNameStream.str();

    //writing image
    stbi_write_png(fileName.c_str(), nx, ny, 3, image_buffer, nx*3);
    //stbi_write_bmp(fileName.c_str(), nx, ny, 3, image_buffer);
    free(image_buffer);
}

//material outdoor tests
hitable *scene1(camera* cam, int nx, int ny)
{
    //camera settings
    vec3 lookfrom = vec3(4, 0.5, 3);
    vec3 lookat = vec3(0.5, 0, -1);
    //camera settings
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1;
    cam->setup(lookfrom, lookat, vec3(0, 1, 0), 25, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    texture *checker = new checker_texture(new constant_texture(vec3(0.1, 0.1, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    lambertian *mat1 = new lambertian(checker);
    metal *mat2 = new metal(vec3(0.9, 0.1, 0.1), 0.01);
    metal *mat4 = new metal(vec3(0.8, 0.6, 0.3), 0.1);
    dielectric *mat3 = new dielectric(1.5);

    hitable **list;
    list = (hitable **)malloc(sizeof(hitable *) * 6);
    list[0] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(checker)); //ground
    list[1] = new sphere(vec3(0, 0, -1), 0.5, mat3);
    list[2] = new sphere(vec3(1, 0, -1), 0.5, mat2);
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, mat1);
    list[4] = new moving_sphere(vec3(2, 0.2, -1), vec3(2, 0, -1), 0, 1, 0.5, mat2);
    list[5] = new sphere(vec3(-2, 0, -1), 0.5, mat4);
    hitable *world = new hitable_list(list, 6);
    return world;
}

//texture, rectangles and lights
hitable *scene2(camera* cam, int nx, int ny)
{
    //camera settings
    vec3 lookfrom = vec3(12, 2.5, 0);
    vec3 lookat = vec3(0, 1.5, 0);
    //camera settings
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.0;
    cam->setup(lookfrom, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    //texture loading
    int texx, texy, texn;
    unsigned char *tex_data = stbi_load("earthmap.jpg", &texx, &texy, &texn, 0);
    material *map = new lambertian(new image_texture(tex_data, texx, texy));

    hitable **list;
    list = (hitable **)malloc(sizeof(hitable *) * 7);
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5)))); //ground
    list[1] = new sphere(vec3(0, 2, 0), 2, map);
    list[2] = new sphere(vec3(1, 1, -3.1), 1, new dielectric(1.5));
    list[3] = new sphere(vec3(1, 1, 3), 0.5, new metal(vec3(0.8, 0.6, 0.3), 0.01));
    list[4] = new sphere(vec3(0, 9, 0), 2, new diffuse_light(new constant_texture(vec3(1, 2, 10))));
    list[5] = new xy_rect(0, 3, 0.5, 4, -4, new diffuse_light(new constant_texture(vec3(1, 1, 1))));
    list[6] = new xy_rect(0, 2, 1, 2.5, 4, new diffuse_light(new constant_texture(vec3(1, 0.2, 0))));
    hitable *world = new hitable_list(list, 7);
    return world;
}

//indoor and lights
hitable *scene3(camera* cam, int nx, int ny)
{
    //camera settings
    vec3 lookfrom = vec3(278, 278, -800);
    vec3 lookat = vec3(278, 278, 0);
    //camera settings
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.0;
    cam->setup(lookfrom, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    int texx, texy, texn;
    unsigned char *tex_data = stbi_load("earthmap.jpg", &texx, &texy, &texn, 0);
    material *map = new lambertian(new image_texture(tex_data, texx, texy));

    hitable **list;
    list = (hitable **)malloc(sizeof(hitable *) * 6);
    list[0] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, new lambertian(new constant_texture(vec3(0, 1, 0)))));
    list[1] = new yz_rect(0, 555, 0, 555, 0, new lambertian(new constant_texture(vec3(1, 0.05, 0.05))));
    list[2] = new flip_normals(new xz_rect(20, 20 + 200, 534 - 200, 534, 554, new diffuse_light(new constant_texture(vec3(2, 2, 2)))));
    list[3] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, new lambertian(new constant_texture(vec3(1, 1, 1)))));
    list[4] = new xz_rect(0, 555, 0, 555, 0, new lambertian(new constant_texture(vec3(1, 1, 1))));
    list[5] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, new lambertian(new constant_texture(vec3(1, 1, 1)))));
    hitable *world = new hitable_list(list, 6);
    return world;
}

int main(int argc, const char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();

    // image definition
    const int nx = 1200; //resolution horizontale
    const int ny = 600; //resolution verticale
    const int ns = 300; // samples
    camera cam;
    
    cout << "\n---- beggining render at resolution " << nx << "*" << ny << "px with " << ns << " samples ----\n";
    
    //load scene
    hitable *world = scene2(&cam, nx, ny);
    //rendering
    render_frame(cam, vec3(nx, ny, ns), world);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    cout << "--- Time to render: " << diff.count() << " s ---\n";
    return 0;
}