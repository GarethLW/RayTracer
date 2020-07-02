#pragma once
#include "Light.h"
#include "Vec3.h"
class directionalLight :
	public Light
{
private: 
	Vec3 direc;
	Color is;
public:
	directionalLight(Vec3 d, Color c);
	~directionalLight();
	Color getIs();
	Vec3 getVec(Vec3 ptIntersection);
};

