#include "Ray.h"



Ray::Ray() {}
Ray::Ray(Vec3 i, Vec3 j) : origin(i), direc(j) {}
Ray::~Ray() {}
Vec3 Ray::getDirec() { return direc; }
Vec3 Ray::getOrigin() { return origin; }
