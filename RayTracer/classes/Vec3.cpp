#include "Vec3.h"
#include <cmath>


Vec3::Vec3() : x(0), y(0), z(0) {}
Vec3::Vec3(float a, float b, float c) : x(a), y(b), z(c) {}


Vec3::~Vec3()
{
}

Vec3 Vec3::operator - (Vec3 v) {
	return Vec3(x - v.getX(), y - v.getY(), z - v.getZ());
}
Vec3 Vec3::operator + (Vec3 v) {
	return  Vec3(x + v.getX(), y + v.getY(), z + v.getZ());
}
Vec3 Vec3::operator * (float t) {
	return  Vec3(x*t, y*t, z*t);
}
Vec3 Vec3::operator/(float d)
{
	return Vec3(x/d,y/d,z/d);
}
Vec3 Vec3::cross(Vec3 u, Vec3 v)
{
	float uvi = u.getY() * v.getZ() - v.getY() * u.getZ();
	float uvj = v.getX() * u.getZ() - u.getX() * v.getZ();
	float uvk = u.getX() * v.getY() - v.getX() * u.getY();
	return  Vec3(uvi, uvj, uvk);
}
Vec3 Vec3::cross(Vec3 v)
{
	float uvi = y * v.getZ() - v.getY() * z;
	float uvj = v.getX() * z - x * v.getZ();
	float uvk = x * v.getY() - v.getX() * y;
	return  Vec3(uvi, uvj, uvk);
}
float Vec3::dot(Vec3 v, Vec3 b)
{
	return (v.getX()*b.getX() + v.getY()*b.getY() + v.getZ()*b.getZ());
}
float Vec3::dot(Vec3 b)
{
	return (x*b.getX() + y*b.getY() + z*b.getZ());
}
Vec3 Vec3::normalize() const
{
	float dist = sqrt(x*x + y*y + z*z);
	return Vec3(x/dist, y/dist, z/dist);
}
float Vec3::magnatude()
{
	return sqrt(x*x + y * y + z * z);
}
float Vec3::getX() { return x; }
float Vec3::getY() { return y; }
float Vec3::getZ() { return z; }

void Vec3::setX(float newX)
{
	x = newX;
}

void Vec3::setY(float newY)
{
	y = newY;
}

void Vec3::setZ(float newZ)
{
	z = newZ;
}
