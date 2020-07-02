#include "PointLight.h"


PointLight::PointLight(Vec3 p, Color c)
{
	pos = p;
	is = c;
	kc = 0;
	kl = 0;
	kq = 0.0035;
}

PointLight::PointLight(Vec3 p, Color c, float kconst, float klin, float kquad)
{
	pos = p;
	is = c;
	kc = kconst;
	kl = klin;
	kq = kquad;
}

PointLight::~PointLight()
{
}

Vec3 PointLight::getPos()
{
	return pos;
}

Color PointLight::getIs()
{
	return is;
}

Vec3 PointLight::getVec(Vec3 ptIntersection)
{
	return pos - ptIntersection;
}

float PointLight::getKc()
{
	return kc;
}
float PointLight::getKl()
{
	return kl;
}

float PointLight::getKq()
{
	return kq;
}
