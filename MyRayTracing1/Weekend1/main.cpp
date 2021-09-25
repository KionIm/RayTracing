#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "sphere.h"
#include "rectangle.h"
#include "box.h"
#include "moving_sphere.h"
#include "hittable_list.h"
#include "material.h"
#include "camera.h"
#include "pdf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include "ray.h"

namespace patch
{
    template < typename T > std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

float hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - (radius * radius);
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0) {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

//vec3 random_in_unit_sphere1() {
//    vec3 p;
//    do {
//        p = 2.0 * vec3((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX) - vec3(1, 1, 1);
//    } while(p.length_squared() >= 1.0);
//    return p;
//}

vec3 color1(const ray& r,hittable *world,int depth){
        hit_record rec;
        if (world->hit(r, 0.001, 1000.0, rec)) {
            ray scattered;
            vec3 attenuation;
            vec3 emitted = rec.mat_ptr->emitted(r,rec,rec.u, rec.v, rec.p);
            float pdf;
            float pdf_val;
            vec3 albedo;
            //bool recMat = rec.mat_ptr->scatter(r, rec, albedo, scattered,pdf);
            bool recMat = rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val);
            if (depth < 50 && recMat) {
                /*cosine_pdf p(rec.normal);
                vec3 dirp = p.generate();*/

                //hittable* light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
                hittable* light_shape = new xz_rect(213-150, 343-150, 227-150, 332-150, 554, 0);
                //hittable* light_shape = new translate(new xz_rect(213, 343, 227, 332, 554, 0), vec3(-100, 0, -100));
                hittable_pdf p0(light_shape, rec.p);
                cosine_pdf p1(rec.normal);
                //mixture_pdf p(&p0, &p1);

                auto p = p0;

                scattered = ray(rec.p, p.generate(), r.time());
                pdf_val = p.value(scattered.direction());

                /*vec3 on_light = vec3(213 + (double)rand() / RAND_MAX * (343 - 213), 554, 227 + (double)rand() / RAND_MAX * (332 - 227));
                vec3 to_light = on_light - rec.p;
                float distance_squared = to_light.length_squared();
                to_light = unit_vector(to_light);
                if (dot(to_light, rec.normal) < 0)
                    return emitted;
                float light_area = (343 - 212) * (332 - 227);
                float light_cosine = fabs(to_light.y());
                if (light_cosine < 0.000001)
                    return emitted;
                pdf = distance_squared / (light_cosine * light_area);
                scattered = ray(rec.p, to_light, r.time());*/

                float Scat = rec.mat_ptr->scattering_pdf(r, rec, scattered);

                return emitted + albedo * Scat * color1(scattered, world, depth + 1) / pdf_val;
                //return emitted + albedo * Scat * color1(scattered, world, depth + 1)/pdf;
                //return emitted + attenuation * color1(scattered, world, depth + 1);
            }
            else {
                return emitted;
            }
            //vec3 target = rec.p + rec.normal + random_in_unit_sphere1();  // t_maxÇí¥Ç¶Ç»Ç¢Ç∆è’ìÀÇ≈Ç´Ç»Ç≠Ç»Ç¡ÇΩéûÇ…ÅAelseÇ÷çsÇ≠
            //return 0.5 * color1(ray(rec.p, target - rec.p), world);
            //return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
        }
        else
            return vec3(0, 0, 0);
}

int main() {
    int nx = 500;
    int ny = 500;
    int ns = 50;
    FILE* fp;
    errno_t err;

     /*vec3 lower_left_corner(-2.0,-1.0,-1.0);
     vec3 horizontal(4.0,0.0,0.0);
     vec3 vertical(0.0,2.0,0.0);
     vec3 origin(0.0,0.0,0.0);*/

    
    
     

     texture* red = new constant_texture(vec3(0.65, 0.05, 0.05));
     texture* white = new constant_texture(vec3(0.83, 0.83, 0.83));
     texture* green = new constant_texture(vec3(0.12, 0.45, 0.12));
     texture* orange = new constant_texture(vec3(0.5, 0.15, 0.05));
     texture* yellow = new constant_texture(vec3(0.35, 0.25, 0.05));
     texture* light = new constant_texture(vec3(15, 15, 15));

     texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
         new constant_texture(vec3(0.9, 0.9, 0.9)));
     texture* pertext = new noise_texture(0.1);

     int nx2, ny2, nn2;
     nx2 = 200;
     ny2 = 100;
     unsigned char* tex_data = stbi_load("earthmap.jpg", &nx2, &ny2, &nn2, 0);

     shared_ptr<material> redM = make_shared<lambertian>(red);
     shared_ptr<material> orangeM = make_shared<lambertian>(orange);
     shared_ptr<material> yellowM = make_shared<lambertian>(yellow);
     shared_ptr<material> whiteM = make_shared<lambertian>(white);
     shared_ptr<material> greenM = make_shared<lambertian>(green);
     shared_ptr<material> lightM = make_shared<diffuse_light>(light);

     shared_ptr<material> texM = make_shared<lambertian>(new image_texture(tex_data, nx2, ny2));
     shared_ptr<material> checkerM = make_shared<lambertian>(checker);
     shared_ptr<material> pertextM = make_shared<lambertian>(pertext);
     shared_ptr<material> metalM = make_shared<metal>(vec3(0.8, 0.6, 0.2));


     const int objectNum = 8;
     hittable* list[objectNum];

     //list[0] = new translate(new xz_rect(213, 343, 227, 332, 554, lightM), vec3(-100, 0, -100));
     list[0] = new xz_rect(213 - 150, 343 - 150, 22 - 150, 332 - 150, 554, lightM);
     list[1] = new xz_rect(0, 555, 0, 555, 0, greenM);
     list[2] = new translate(new box(vec3(0, 0, 0), vec3(15, 90, 120), redM), vec3(265, 0,295));
     list[3] = new translate(new box(vec3(0, 0, 0), vec3(120, 90, 15), orangeM), vec3(265, 0, 280));
     list[4] = new translate(new box(vec3(0, 0, 0), vec3(15, 90, 120), yellowM), vec3(385, 0, 280));
     list[5] = new translate(new box(vec3(0, 0, 0), vec3(120, 90, 15), whiteM), vec3(280, 0, 400));
     //list[2] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(15, 120, 90), whiteM),45), vec3(265, 0, 295));
     list[6] = new sphere(vec3(280, 30, 185), 30, pertextM);
     list[7] = new sphere(vec3(380, 50, 170), 50, texM);
     //list[8] = new sphere(vec3(230, 40, 230), 40, metalM);
     
     /*list[0] = new xz_rect(213, 343, 227, 332, 554, lightM);
     list[1] = new yz_rect(0, 555, 0, 555, 0, redM);
     list[2] = new yz_rect(0, 555, 0, 555, 555, greenM);
     list[3] = new xz_rect(0, 555, 0, 555, 0, whiteM);
     list[4] = new xy_rect(0, 555, 0, 555, 555, whiteM);
     list[5] = new xz_rect(0, 555, 0, 555, 555, whiteM);

     list[6] = new translate(new rotate_y(new box(vec3(0, 0,0), vec3(165, 165, 165), whiteM), -18), vec3(130, 0, 65));
     list[7] = new translate(new rotate_y(new box(vec3(0, 0,0), vec3(165, 330, 165), whiteM), 15), vec3(265, 0, 295));*/

     

     
     
     //shared_ptr<material> sphere_material4 = make_shared<metal>(vec3(0.8, 0.8, 0.8));

     ////list[0] = new sphere(vec3(0, 0, -3),0.5, sphere_material3);
     //list[0] = new sphere(vec3(0, -100.5, -1), 100,sphere_material2);
     //list[2] = new sphere(vec3(1, 3, 3), 2, sphere_material6);
     //list[1] = new xy_rect(3, 5, 1, 3, -2, sphere_material6);

     hittable *world = new hittable_list(list, objectNum);

     std::string nx_s = patch::to_string(nx);
     std::string ny_s = patch::to_string(ny);
     std::string nxy_s =  nx_s + " " + ny_s +"\n";
     char const* nxy_char = nxy_s.c_str();

    err = fopen_s(&fp, "test.ppm", "w");
    fprintf(fp, "P3\n");
    fprintf(fp,nxy_char);
    fprintf(fp, "255\n");

    /*vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);*/
    vec3 lookfrom(100, 400, -100);
    vec3 lookat(278, 78, 150);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    //camera cam(vec3(10,4,0), vec3(0,0,-1), vec3(0,1,0),20,float(nx)/float(ny),0,10,0.0,1.0);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; j--) {
        std::cout << j << "\n";
        for (int i = 0; i < nx; i++) {
            vec3 col(0.0, 0.0, 0.0);
            for (int s = 0;s < ns; s++)
            {
                float u = float(i + (double)rand() / RAND_MAX) / float(nx);
                float v = float(j + (double)rand() / RAND_MAX) / float(ny);

                ray r = cam.get_ray(u, v);

                //ray r(origin, lower_left_corner + u * horizontal + v * vertical);
                vec3 p = r.at(2.0);
                col += color1(r,world,0);
            }
            col /= float(ns);

            int ir = int(255.99 * sqrt(col[0]));
            int ig = int(255.99 * sqrt(col[1]));
            int ib = int(255.99 * sqrt(col[2]));
            //std::cout << ir << " " << ig << " " << ib << "\n";

            std::string ir_s = patch::to_string(ir);
            std::string ig_s = patch::to_string(ig);
            std::string ib_s = patch::to_string(ib);

            std::string s = ir_s + " " + ig_s + " " + ib_s + "\n";

            char const* s_char = s.c_str();

            fprintf(fp, s_char);
        }
    }
}