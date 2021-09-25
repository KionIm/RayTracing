#ifndef BVH_H
#define BVH_H

#include "hittable_list.h"
#include "aabb.h"

#include <algorithm>


class bvh_node : public hittable {
public:
    bvh_node();

    bvh_node(hittable **list, int n, float time0, float time1); 

    /*bvh_node(
        const std::vector<shared_ptr<hittable>>& src_objects,
        size_t start, size_t end, double time0, double time1);*/

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
    hittable *left;
    hittable *right;

   /* shared_ptr<hittable> left;
    shared_ptr<hittable> right;*/
    aabb box;
};

int box_x_compare(const void * a, const void * b) {
    aabb box_left;
    aabb box_right;
    hittable *ah = *(hittable**)a;
    hittable *bh = *(hittable**)b;

    if(!ah->bounding_box(0,0,box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";
    if (box_left.min().x() - box_right.min().x() < 0.0 )
        return -1;
    else
        return 1;
}

int  box_y_compare(const void* a, const void* b) {
    aabb box_left;
    aabb box_right;
    hittable* ah = *(hittable**)a;
    hittable* bh = *(hittable**)b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";
    if (box_left.min().y() - box_right.min().y() < 0.0)
        return -1;
    else
        return 1;
}

int  box_z_compare(const void* a, const void* b) {
    aabb box_left;
    aabb box_right;
    hittable* ah = *(hittable**)a;
    hittable* bh = *(hittable**)b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";
    if (box_left.min().z() - box_right.min().z() < 0.0)
        return -1;
    else
        return 1;
}

bvh_node::bvh_node(hittable **l, int n, float time0, float time1) {
    int axis = int(3 * (double)rand() / RAND_MAX);
    if (axis == 0)
        qsort(l, n, sizeof(hittable *), box_x_compare);
    else if( axis == 1)
        qsort(l, n, sizeof(hittable*), box_y_compare);
    else
        qsort(l, n, sizeof(hittable*), box_z_compare);
    if (n == 1)
        left = l[0];
        right = l[0];
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, rec);
        bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
        }
        else if (hit_right) {
            rec = right_rec;
            return true;
        }
        else
            return false;
    }
    else return false;

}

bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}

#endif
