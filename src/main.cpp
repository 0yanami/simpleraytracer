#ifdef __clang__
#define STBIWDEF static inline
#endif
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION


#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <random>
#include <memory>
#include <omp.h>
#include <cstdlib>

#include"moving_sphere.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include "stb_image_write.h"

using namespace std;

vec3 color(const ray &r, hitable *world, int depth){
    hit_record rec;

    if (world->hit(r, 0.001, 999999.9, rec)){
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            return attenuation*color(scattered, world, depth+1);
        }
        else{
            return vec3(0,0,0);
        }
    }
    else{
        vec3 unit_direction = r.direction().unit_vector();
        float t = 0.5 * (unit_direction.y() + 1.0);
        return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
        
    }
}

void render_frame(int frame_num, vec3 cam_pos, vec3 res, hitable *world){
    //Anti Aliasing rng generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);

    int nx = int(res.x());
    int ny = int(res.y());
    int ns = int(res.z());

    auto image_buffer = (unsigned char*)malloc(nx*ny*3 * sizeof(unsigned char));
    
    //init camera 
    vec3 lookfrom = vec3(3,3,2);
    vec3 lookat = vec3(0,0,-1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.1;
    
    camera cam(lookfrom, lookat, vec3(0,1,0), 25,float(nx)/float(ny), aperture, dist_to_focus,0.0,1.0);
    
    #pragma omp parallel for
    for (int i = 0; i < ny*nx; i++){
        vec3 col(0, 0, 0);
        for (int s = 0; s < ns; s++){
            float u = float(i%int(nx) + dis(gen)) / float(nx);
            float v = 1.0 - float(int(i/nx)+1 + dis(gen)) / float(ny);
            ray r = cam.get_ray(u, v);
            vec3 p = r.point_at_parameter(2.0);
            col += color(r, world,0);
        }
        col /= float(ns);
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
        image_buffer[i*3 + 0] = int(255.99 * col[0]);
        image_buffer[i*3 + 1] = int(255.99 * col[1]);
        image_buffer[i*3 + 2] = int(255.99 * col[2]);
    }

    //file path
    ostringstream fileNameStream("out");
    fileNameStream << "output/out" << frame_num << ".bmp";
    string fileName = fileNameStream.str();

    //writing image
    stbi_write_bmp( fileName.c_str() , nx, ny, 3, image_buffer);
    free(image_buffer);
}

hitable *scene1(){
    lambertian *mat1    = new lambertian(vec3(0.1,0.2,0.5));
    metal *mat2         = new metal(vec3(0.8,0.6,0.2),0.01);
    dielectric *mat3    = new dielectric(1.5);
    
    hitable** list;
    list = (hitable**)malloc(sizeof(hitable*)*5);
    list[0] = new sphere(vec3(0, -100.5, -1) , 100,  new lambertian(vec3(0.8,0.8,0)) );//ground
    list[1] = new sphere(vec3(0, 0, -1)      , 0.5, mat1);
    list[2] = new sphere(vec3(1, 0, -1)      , 0.5, mat2);
    list[3] = new sphere(vec3(-1, 0, -1)     , 0.5, mat3);
    list[4] = new moving_sphere(vec3(0, 0, 0), vec3(0, 0, 0.5), 0 , 0.5 , 0.5, mat2);
    hitable *world = new hitable_list(list, 5);
    return world;
}

int main(int argc, const char *argv[]){
    auto start = std::chrono::high_resolution_clock::now();

    // image definition
    const int nx = 1000;//resolution horizontale
    const int ny = 500;//resolution verticale
    const int ns = 30;// samples
    const int frames = 1;
    vec3 pos_cam = vec3(0,0.5,4);
    vec3 cam_dir = vec3(0,0,0); //direction de l'anim
    cout << "\n---- beggining render at resolution "<< nx <<  "*" << ny << "px with "<< ns << " samples ----\n";

    //list des objets
    //std::shared_ptr<hitable> list[5]; // std::make_shared<sphere>(...)

    vec3 frame_pos_cam = pos_cam;
    int count = 0;
    


 
    
    hitable* world = scene1();
    
    for(int i=0; i<frames;i++){
        auto start2 = chrono::high_resolution_clock::now();
        frame_pos_cam = ( pos_cam + cam_dir*float(float(i)/float(frames)) ) ;
        render_frame(i, frame_pos_cam, vec3(nx,ny,ns), world);
        auto end2 = chrono::high_resolution_clock::now();
        chrono::duration<double> diff2 = end2-start2;
        cout << "render of frame " <<  count+1 << "/" << frames << " took " << diff2.count() << "s\n";
        count++;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end-start;
    cout << "--- Total time to render sequence: " << diff.count() << " s ---\n";
    return 0;
}