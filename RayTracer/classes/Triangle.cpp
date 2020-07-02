#include "Triangle.h"
#include "Vec3.h"
#include "Ray.h"
#include "Plane.h"


Triangle::Triangle(Vec3 a, Vec3 b, Vec3 c) : pa(a), pb(b), pc(c) 
{
	Vec3 v1(pa - pb);
	Vec3 v2(pa - pc);
	normal = v1.cross(v2).normalize();
}

Triangle::~Triangle()
{
}

bool Triangle::intersect(Ray ray, float &t)
{
	Vec3 normal = (pa - pb).cross(pa - pc);
	Plane plane(normal, pa); // either the normal is not normal or pa is not on the plane or planeintersect is broken
	if (!plane.intersect(ray, t)) return false;
	Vec3 x = (ray.getDirec()*t) + ray.getOrigin();

	float checkA = normal.dot((pb - pa).cross(x - pa)); // check magnitude instead
	if (!(checkA > 0)) return false;

	float checkB = normal.dot((pc - pb).cross(x - pb));
	if (!(checkB > 0)) return false;

	float checkC = normal.dot((pa - pc).cross(x - pc));
	if (!(checkC > 0)) return false;

	return true;
}

void Triangle::getMinMax(Vec3 & thisMin, Vec3 & thisMax)
{
	thisMin.setX(std::min(pa.getX(), std::min(pb.getX(), pc.getX())));
	thisMin.setY(std::min(pa.getY(), std::min(pb.getY(), pc.getY())));
	thisMin.setZ(std::min(pa.getZ(), std::min(pb.getZ(), pc.getZ())));

	thisMax.setX(std::max(pa.getX(), std::max(pb.getX(), pc.getX())));
	thisMax.setY(std::max(pa.getY(), std::max(pb.getY(), pc.getY())));
	thisMax.setZ(std::max(pa.getZ(), std::max(pb.getZ(), pc.getZ())));
}

Vec3 Triangle::getNormal(Vec3 point)
{
	return normal;
}

Color Triangle::getColor()
{
	return Color(10, 255, 125);
}

float Triangle::getKa()
{
	return 0.5f;
}

float Triangle::getKs()
{
	return 0.8f;
}

float Triangle::getKd()
{
	return 0.7f;
}

int Triangle::getAlpha()
{
	return 70;
}

Vec3 Triangle::getPos()
{
	return (pa + pb + pc) / 3;
}
