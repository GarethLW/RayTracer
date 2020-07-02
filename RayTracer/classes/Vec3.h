#pragma once

class Vec3
{
private:
	float x, y, z;
public:
	Vec3();
	~Vec3();
	Vec3(float a, float b, float c);
	Vec3 operator - (Vec3 v);
	Vec3 operator + (Vec3 v);
	Vec3 operator * (float t);
	Vec3 operator / (float d);
	Vec3 cross(Vec3 u, Vec3 v);
	Vec3 cross(Vec3 v);
	float dot(Vec3 v, Vec3 b);
	float dot(Vec3 b);
	Vec3 normalize() const;
	float magnatude();
	float getX();
	float getY();
	float getZ();
	void setX(float newX);
	void setY(float newY);
	void setZ(float newZ);
};