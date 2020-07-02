#pragma once

#include "SceneObject.h"
#include "Vec3.h"
#include "Ray.h"
#include "Color.h"

class Sphere : public SceneObject
{
private:
	Vec3 center;
	Color color;
	float radius;
	float ka, ks, kd;
	int alpha;
	float trans;
public:
	Sphere(Vec3 c, float r, const float kAmbient);
	Sphere(Vec3 c, float r, Color clr, const float kAmbient, const float kSpec, const float kDiffuse, const int a);
	~Sphere();
	bool intersect(Ray ray, float &t);
	Vec3 getNormal(Vec3 point);
	Color getColor();
	float getKa();
	float getKs();
	float getKd();
	int getAlpha();
	Vec3 getPos();
	void setGlass(bool g);
	void getMinMax(Vec3 &thisMin, Vec3 &thisMax);
};

