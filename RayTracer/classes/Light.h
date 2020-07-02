#pragma once

#include "Color.h"
#include "Vec3.h"

class Light
{
public:
	Light();
	~Light();
	virtual Color getIs() = 0;
	virtual Vec3 getVec(Vec3 ptIntersection) = 0;
};

