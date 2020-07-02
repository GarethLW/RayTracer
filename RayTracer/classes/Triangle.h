#pragma once

#include <algorithm>
#include "SceneObject.h"
#include "Vec3.h"
#include "Ray.h"

class Triangle : public SceneObject
{
private:
	Vec3 pa;
	Vec3 pb;
	Vec3 pc;
	Vec3 normal;
public:
	Triangle(Vec3 a, Vec3 b, Vec3 c);
	~Triangle();
	bool intersect(Ray ray, float &t);
	void getMinMax(Vec3 &thisMin, Vec3 &thisMax);

	Vec3 getNormal(Vec3 point);
	Color getColor();
	float getKa();
	float getKs();
	float getKd();
	int getAlpha();
	Vec3 getPos();
};

