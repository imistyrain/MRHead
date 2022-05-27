#ifndef SPHERE_H
#define SPHERE_H

#include"hittable.h"
#include"vec3.h"

class sphere :public hittable {
public:
	sphere() { }
	sphere(point3 cen, double r) :center(cen), radius(r) { }

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
public:
	point3 center;
	double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center;
	double a = r.direction().length_squared();
	double half_b = dot(r.direction(), oc);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant > 0) {
		double root = sqrt(discriminant);
		double t = (-half_b - root) / a;
		if (t > t_min&& t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			return true;
		}
		t = (-half_b + root) / a;
		if (t > t_min&& t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			return true;
		}
	}
	return false;
}

#endif // ! SPHERE_H