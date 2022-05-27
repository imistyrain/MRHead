#ifndef COLOR_H
#define COLOR_H

#include"vec3.h"
#include "rtweekend.h"
#include<iostream>

void write_color(std::ostream& out, color pixel_color) {
	//写下每个颜色分量转换后的值[0,255]
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

void write_color(std::ostream& out, color pixel_color,int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();
	//根据样本数对颜色取平均值
	auto scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif // !COLOR_H

