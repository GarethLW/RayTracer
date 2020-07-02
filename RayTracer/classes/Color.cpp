#include "Color.h"
#include <algorithm>

Color::Color() { r = g = b = 0; }
Color::Color(int i, int j, int k) { r = i, g = j, b = k; }
Color::~Color() {}
Color Color::average(Color c2)
{
	return Color((r+c2.getR())/2, (g + c2.getG()) / 2, (b + c2.getB()) / 2);
}
Color Color::blend(Color c2, float ratio)
{
	float ratio2 = 1 - ratio;
	return Color (r * ratio + c2.getR() * ratio2 , g*ratio + c2.getG()*ratio2 , b*ratio + c2.getB()*ratio2);
}
Color Color::operator/(float t)
{
	return Color(r/t, g/t, b/t);
}
Color Color::operator*(float t)
{
	if (t < 0) return Color(0, 0, 0);
	return Color(r*t, g*t, b*t);
}
Color Color::operator*(Color c2)
{
	return Color(c2.getR()*r, c2.getG()*g, c2.getB()*b);
}
Color Color::operator / (Color c2)
{
	return Color(c2.getR()/r, c2.getG()/g, c2.getB()/b);
}
Color Color::operator+(Color c2)
{
	int newR = std::min(std::max(r + c2.getR(), 0), 255);
	int newG = std::min(std::max(g + c2.getG(), 0), 255);
	int newB = std::min(std::max(b + c2.getB(), 0), 255);
	return Color(newR, newG, newB);
}
int Color::getR() { return r; }
int Color::getG() { return g; }
int Color::getB() { return b; }