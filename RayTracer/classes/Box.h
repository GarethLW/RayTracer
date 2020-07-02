#pragma once

#include "SceneObject.h"
#include "Vec3.h"
#include "Ray.h"
#include "Color.h"

class Box : public SceneObject
{
private:
	Vec3 min;
	Vec3 max;
	Color color;
	float ka, ks, kd;
	int alpha;
	float trans;
public:
	Box(Vec3 min, Vec3 max, float ambK);
	~Box();
	bool intersect(Ray ray, float &t);
	Vec3 getNormal(Vec3 point);
	Color getColor();
	float getKa();
	float getKs();
	float getKd();
	int getAlpha();
	Vec3 getPos();
	void getMinMax(Vec3 &thisMin, Vec3 &thisMax);
};

