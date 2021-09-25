#ifndef TORUS_H
#define TORUS_H

#include "hittable.h"
#include "material.h"
#include "aabb.h"


class torus : public hittable {
public:
    torus() {}

    torus(point3 cen, float r1,float r2, shared_ptr<material> m)
        : center(cen), radius1(r1),radius2(r2), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(float t0, float t1, aabb& box) const override;

public:
    point3 center;
    float radius1;
    float radius2;
    shared_ptr<material> mat_ptr;
};

bool torus::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    vec3 rpos = r.origin();

    vec3 dv = vec3(rpos.x() - center.x(), 0, rpos.z() - center.z());
    float dx = dv.length() - radius1;
    float dy = rpos.y() - center.y();
    float d = sqrt(dx * dx + dy*dy);

    if (d > radius2) {
        return false;
    }



}

#endif