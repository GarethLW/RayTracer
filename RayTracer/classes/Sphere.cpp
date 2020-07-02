#include "Sphere.h"
#include "Ray.h"
#include <cmath>
#include "Vec3.h"
#include "Color.h"



Sphere::Sphere(Vec3 c, float r, const float kAmbient) 
{ 
	ka = kAmbient;
	kd = 0.5f;
	ks = 0.5f;
	alpha = 70;
	center = c;
	radius = r;
	color = Color(255, 137, 161);
}

Sphere::Sphere(Vec3 c, float r, Color clr, const float kAmbient, const float kSpec, const float kDiffuse, const int a) 
{ 
	ka = kAmbient; 
	kd = kDiffuse;
	ks = kSpec;
	alpha = a;
	center = c; 
	radius = r; 
	color = clr;
	trans = 0;
}


Sphere::~Sphere()
{
}

bool Sphere::intersect(Ray ray, float &t)
{
	Vec3 l = ray.getDirec();
	l = l.normalize();
	Vec3 o = ray.getOrigin();
	float toroot = pow(l.dot(o - center), 2) - pow((o - center).magnatude(), 2) + radius * radius;
	
	if (toroot < 0) return false;
	float base = -1 * (l.dot(o - center));
	if (toroot > 0)
	{
		toroot = sqrt(toroot);
		float t0 = base - toroot;
		float t1 = base + toroot;
		t = (t0 < t1) ? t0 : t1;
		if (abs(t0) < 0.1) t = t1;
		else if (abs(t1) < 0.1) t = t0;
		return true;
	}
	if (abs(base) < 0.1) return false;
	t = base;
	return true;
}

Vec3 Sphere::getNormal(Vec3 point)
{
	return (point - center);
}

Color Sphere::getColor()
{
	return color;
}

float Sphere::getKa()
{
	return ka;
}

float Sphere::getKs()
{
	return ks;
}

float Sphere::getKd()
{
	return kd;
}

int Sphere::getAlpha()
{
	return alpha;
}

Vec3 Sphere::getPos()
{
	return center;
}

void Sphere::setGlass(bool g)
{
	SceneObject::setGlass(g);
}

void Sphere::getMinMax(Vec3 & thisMin, Vec3 & thisMax)
{
	thisMin.setX(center.getX() - radius);
	thisMin.setY(center.getY() - radius);
	thisMin.setZ(center.getZ() - radius);

	thisMax.setX(center.getX() + radius);
	thisMax.setY(center.getY() + radius);
	thisMax.setZ(center.getZ() + radius);
}
