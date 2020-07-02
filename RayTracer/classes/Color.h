#pragma once

class Color
{
private:
	int r, g, b;
public:
	Color();
	Color(int i, int j, int k);
	~Color();
	Color average(Color c2);
	Color blend(Color c2, float ratio);
	Color operator / (float t);
	Color operator * (float t);
	Color operator * (Color c2);
	Color operator + (Color c2);
	Color Color::operator / (Color c2);
	int getR();
	int getG();
	int getB();
};

