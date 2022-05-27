#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include<cmath>
#include<cstdlib>
#include<limits>
#include<memory>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//常量
const double infinity = std::numeric_limits<double>::infinity(); //无穷大
const double pi = 3.1415926535897932385;

//函数
inline double degrees_to_radians(double degrees) { //度数到弧度
    return degrees * pi / 180.0;
}

inline double random_double() {
    //[0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    //[min,max)
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

//公共头文件
#include"ray.h"
#include"vec3.h"

#endif // !RTWEEKEND_H