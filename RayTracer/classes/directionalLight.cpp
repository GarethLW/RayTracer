#include "directionalLight.h"
#include "Vec3.h"


directionalLight::directionalLight(Vec3 d, Color c)
{
	direc = d;
	is = c;
}

directionalLight::~directionalLight()
{
}

Color directionalLight::getIs()
{
	return is;
}

Vec3 directionalLight::getVec(Vec3 ptIntersection)
{
	return direc*-1;
}
