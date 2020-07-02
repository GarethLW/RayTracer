#include "Plane.h"
#include "Vec3.h"
#include "Ray.h"

Plane::Plane(Vec3 n, Vec3 p) 
{ 
	normal = n; 
	position = p;
	ka = 0.5;
	kd = 0.7f;
	ks = 0.1f;
	alpha = 40;
}

Plane::~Plane()
{
}

bool Plane::intersect(Ray ray, float &t)
{
	float denominator = normal.dot(ray.getDirec());

	if (denominator > 1e-6 || denominator < -1e-6)
	{
		t = (position - ray.getOrigin()).dot(normal) / denominator;
		return (t >= 0);
	}
	else return false;
}

Vec3 Plane::getNormal(Vec3 point)
{
	return normal;
}

Color Plane::getColor()
{
	return Color(80,80,80);
}

float Plane::getKa()
{
	return ka;
}

float Plane::getKs()
{
	return ks;
}

float Plane::getKd()
{
	return kd;
}

int Plane::getAlpha()
{
	return alpha;
}

Vec3 Plane::getPos()
{
	return position;
}

void Plane::getMinMax(Vec3 & thisMin, Vec3 & thisMax)
{

}
