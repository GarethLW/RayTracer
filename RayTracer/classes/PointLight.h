#pragma once

#include "Light.h"
#include "Vec3.h"
#include "Color.h"

class PointLight : public Light
{
private:
	Vec3 pos;
	Color is;
	float kc, kl, kq;
public:
	PointLight(Vec3 pos, Color c);
	PointLight(Vec3 p, Color c, float kconst, float klin, float kquad);
	~PointLight();
	Vec3 getPos();
	Color getIs();
	Vec3 getVec(Vec3 ptIntersection);
	float getKc();
	float getKl();
	float getKq();
};

