#ifndef MATERIAL_H
#define MATERIAL_H


#include "rtweekend.h"
#include "hittable.h"
#include "texture.h"
#include "ray.h"

class material {
public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& albedo, ray& scattered, float& pdf
    ) const
    {
        return false;
    };

    virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const 
    {
        return false;
    }

    virtual vec3 emitted(const ray& rt_in,const hit_record& rec,float u, float v, const vec3& p) const {
        return vec3(0, 0, 0);
    }
};

inline vec3 randome_cosine_direction() {
    float r1 = (double)rand() / RAND_MAX;
    float r2 = (double)rand() / RAND_MAX;
    float z = sqrt(1 - r2);
    float phi = 2 * 3.141592 * r1;
    float x = cos(phi) * 2 * sqrt(r2);
    float y = sin(phi) * 2 * sqrt(r2);
    return vec3(x, y, z);
}

class onb
{
public:
    onb() {}
        inline vec3 operator[](int i)const { return axis[i]; }
        vec3 u() const { return axis[0]; }
        vec3 v() const { return axis[1]; }
        vec3 w() const { return axis[2]; }
        vec3 local(float a, float b, float c) const { return a * u() + b * v() + c * w(); }
        vec3 local(const vec3& a) const { return a.x() * u() + a.y() * v() + a.z() * w(); }
        void build_from_w(const vec3&);
        vec3 axis[3];
};

void onb::build_from_w(const vec3& n) {
    axis[2] = unit_vector(n);
    vec3 a;
    if (fabs(w().x()) > 0.9)
        a = vec3(0, 1, 0);
    else
        a = vec3(1, 0, 0);
    axis[1] = unit_vector(cross(w(), a));
    axis[0] = cross(w(), v());
    /*axis[1] = unit_vector(cross(axis[1], a));
    axis[0] = cross(axis[2], axis[1]);*/
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

class metal : public material {

public:
    metal(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    //float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
    //    /*float cosine = dot(rec.normal, unit_vector(scattered.direction()));
    //    if (cosine < 0) cosine = 0;*/
    //    return 0.1 / 3.1415926535;
    //}

public :
    vec3 albedo;
};

vec3 random_in_unit_sphere1() {
    vec3 p;
    do {
        p = 2.0 * vec3((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX) - vec3(1, 1, 1);
    } while (p.length_squared() >= 1.0);
    return p;
}

void get_sphere_uv(const vec3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (phi + 3.1415926535) / (2 * 3.14159265);
    v = (theta + 0.5 * 3.14159265) / (3.14159265);
}

class lambertian : public material {
public:
    lambertian(texture  *a):albedo(a){}

    float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
        float cosine = dot(rec.normal, unit_vector(scattered.direction()));
        if (cosine < 0) cosine = 0;
        return cosine / 3.1415926535;
    }

    //virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& alb, ray& scattered,float& pdf) const
    //{
    //    float u, v;
    //    vec3 target = rec.p + rec.normal + random_in_unit_sphere1();
    //    scattered = ray(rec.p, unit_vector(target - rec.p), r_in.time());
    //    alb = albedo->value(rec.u, rec.v, rec.p);
    //    pdf = dot(rec.normal, scattered.direction()) / 3.1415926535;
    //    return true;
    //}

    bool scatter(const ray& r_in, const hit_record& rec, vec3& alb, ray& scattered, float& pdf) const
    {
        onb uvw;
        uvw.build_from_w(rec.normal);
        vec3 rcd = randome_cosine_direction();
        vec3 direction = uvw.local(rcd);
        //vec3 direction;
        /*do {
            direction = random_in_unit_sphere();
        } while (dot(direction, rec.normal) < 0);*/
        scattered = ray(rec.p, unit_vector(direction), r_in.time());
        alb = albedo->value(rec.u, rec.v, rec.p);
        pdf = 0.5 / 3.1415926535;
        pdf = dot(uvw.w(), scattered.direction()) / 3.1415926535;
        vec3 dirB = rec.normal;
        vec3 dirA = scattered.direction();
        //pdf = dot(unit_vector(direction), unit_vector(direction)) / 3.1415926535;
        return true;
    }

    texture *albedo;

};

class diffuse_light : public material {
public:
    diffuse_light(texture *a) : emit(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, float& pdf) const { return false; }
    virtual vec3 emitted(const ray& r_in,const hit_record& rec,float u, float v, const vec3& p)  const
    {
        if (dot(rec.normal, r_in.direction()) < 0.0)
            return emit->value(u, v, p);
        else
            return vec3(0, 0, 0);
    }
    texture* emit;
};

#endif