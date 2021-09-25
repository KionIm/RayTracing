#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H


//#include "rtweekend.h"

#include "hittable.h"

#include <memory>
#include <vector>


class hittable_list : public hittable {
public:
    hittable_list() {}
    hittable_list(hittable** l, int n) { list = l; list_size = n; }
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(float t0, float t1, aabb& box) const override;

    hittable** list;
    int list_size;

public:
    std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;

    for (int i = 0; i < list_size;i++) {
        if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool hittable_list::bounding_box(float t0, float t1, aabb& box) const {
    if (list_size < 1) return false;
    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0, t1, temp_box);
    if (!first_true)
        return false;
    else
        box = temp_box;
    for (int i = 1; i < list_size; i++) {
        if (list[0]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        }
    }
}

class flip_normals : public hittable {
public :
    flip_normals(hittable *p) : ptr(p) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        if (ptr->hit(r, t_min, t_max, rec)) {
            rec.normal = -rec.normal;
            return true;
        }
        else
            return false;
    }

    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        return ptr->bounding_box(t0, t1, box);
    }

    hittable* ptr;
};

class translate : public hittable {
public:
    translate(hittable *p,const vec3& displacement) : ptr(p),offset(displacement) {}
    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(float t0, float t1, aabb& box) const override;

    hittable* ptr;
    vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(moved_r, t_min, t_max, rec)) {
        rec.p += offset;
        return true;
    }
    else
        return false;
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = aabb(box.min() + offset, box.max() + offset);
        return true;
    }
    else
        return false;
}

class rotate_y : public hittable {
public:
    rotate_y(hittable *p, double angle);

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    hittable *ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
};


rotate_y::rotate_y(hittable * p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}


bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;

    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}


#endif
