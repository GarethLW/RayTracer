#include "Box.h"
#include "Vec3.h"
#include <algorithm>
#include "Ray.h"
#include "Color.h"

Box::Box(Vec3 newMin, Vec3 newMax, float ambk) : min(newMin), max(newMax) 
{ 
	ka = ambk; 
	kd = 0.5f;
	ks = 1.0f;
	alpha = 10;
	color = Color(0, 255, 0);
}

Box::~Box()
{
}

bool Box::intersect(Ray ray, float &t)
{
	float nearX, nearY, nearZ;
	float farX, farY, farZ;
	float t0x = (min.getX() - ray.getOrigin().getX()) / ray.getDirec().getX();
	float t1x = (max.getX() - ray.getOrigin().getX()) / ray.getDirec().getX();
	if (t0x < t1x) {
		nearX = t0x;
		farX = t1x;
	} else {
		nearX = t1x;
		farX = t0x;
	}

	float t0y = (min.getY() - ray.getOrigin().getY()) / ray.getDirec().getY();
	float t1y = (max.getY() - ray.getOrigin().getY()) / ray.getDirec().getY();
	if (t0y < t1y) {
		nearY = t0y;
		farY = t1y;
	}
	else {
		nearY = t1y;
		farY = t0y;
	}

	float t0z = (min.getZ() - ray.getOrigin().getZ()) / ray.getDirec().getZ();
	float t1z = (max.getZ() - ray.getOrigin().getZ()) / ray.getDirec().getZ();
	if (t0z < t1z) {
		nearZ = t0z;
		farZ = t1z;
	}
	else {
		nearZ = t1z;
		farZ = t0z;
	}

	float minTFar = std::min(farX, std::min(farY, farZ));
	float maxTNear =  std::max(nearX, std::max(nearY, nearZ));

	t = maxTNear;

	if (minTFar > maxTNear) return true;
	
	t = maxTNear;
	return false;

}

Vec3 Box::getNormal(Vec3 point)
{
	float m = 0.005; // margin of error

	if (abs(point.getX() - min.getX()) < m) return Vec3(-1, 0, 0);
	else if (abs(point.getX() - max.getX()) < m) return Vec3(1, 0, 0);
	else if (abs(point.getY() - min.getY()) < m) return Vec3(0, -1, 0);
	else if (abs(point.getY() - max.getY()) < m) return Vec3(0, 1, 0);
	else if (abs(point.getZ() - min.getZ()) < m) return Vec3(0, 0, -1);
	else if (abs(point.getZ() - max.getZ()) < m) return Vec3(0, 0, 1);

	return Vec3();
}

Color Box::getColor()
{
	return color;
}

float Box::getKa()
{
	return ka;
}

float Box::getKs()
{
	return ks;
}

float Box::getKd()
{
	return kd;
}

int Box::getAlpha()
{
	return alpha;
}

Vec3 Box::getPos()
{
	return Vec3((min.getX()+max.getX())/2, (min.getY() + max.getY()) / 2, (min.getZ() + max.getZ()) / 2);
}

void Box::getMinMax(Vec3 & thisMin, Vec3 & thisMax)
{
	thisMin.setX(min.getX());
	thisMin.setY(min.getY());
	thisMin.setZ(min.getZ());

	thisMax.setX(max.getX());
	thisMax.setY(max.getY());
	thisMax.setZ(max.getZ());
}
