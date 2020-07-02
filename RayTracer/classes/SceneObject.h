#pragma once

#include "Ray.h"
#include "Vec3.h"
#include "Color.h"

class SceneObject
{
private:
	bool glass, mirror;
	float trans;

public:
	SceneObject();
	~SceneObject();
	virtual bool intersect(Ray ray, float &t) = 0;
	virtual Vec3 getNormal(Vec3 point) = 0;
	virtual Color getColor() = 0;
	virtual float getKa() = 0;
	virtual float getKs() = 0;
	virtual float getKd() = 0;
	virtual int getAlpha() = 0;
	virtual Vec3 getPos() = 0;
	virtual bool isGlass();
	virtual void setGlass(bool g);
	virtual bool isMirror();
	virtual void setMirror(bool m);
	virtual void getMinMax(Vec3 &thisMin, Vec3 &thisMax) = 0;
	virtual void setTrans(float newT);
	virtual float getTrans();
};

