#pragma once

#include "Vec3.h"

class Ray
{
private:
	Vec3 origin;
	Vec3 direc; // direction
public:
	Ray();
	Ray(Vec3 i, Vec3 j);
	~Ray();
	Vec3 getOrigin();
	Vec3 getDirec();
};

