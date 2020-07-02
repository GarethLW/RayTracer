#pragma once

#include "SceneObject.h"
#include "Vec3.h"
#include "Ray.h"

class Plane : public SceneObject
{
private:
	Vec3 position;
	Vec3 normal;
	float ka, ks, kd;
	int alpha;
public:
	Plane(Vec3 n, Vec3 p);
	~Plane();
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

