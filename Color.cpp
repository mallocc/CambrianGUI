#include "Color.h"

namespace gui
{
	Color black = { 0,0,0 };
	Color off_black = { 0.05,0.05,0.05 };
	Color white = { 1,1,1 };
	Color off_white = { .95,.95,.95 };
	Color red = { 1,0,0 };
	Color green = { 0,1,0 };
	Color blue = { 0,0,1 };
	Color cyan = { 0,1,1 };
	Color magenta = { 1,1,0 };
	Color yellow = { 1,0,1 };
	Color grey = { 0.5,0.5,0.5 };
	
	Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
	{
	}

	Color Color::operator+(Color operand)
	{
		Color temp = *this;
		temp.r += operand.r;
		temp.g += operand.g;
		temp.r += operand.b;
		return temp;
	}
	Color& Color::operator+=(Color operand)
	{
		this->r += operand.r;
		this->g += operand.g;
		this->r += operand.b;
		return *this;
	}
	Color Color::operator-(Color operand)
	{
		Color temp = *this;
		temp.r -= operand.r;
		temp.g -= operand.g;
		temp.r -= operand.b;
		return temp;
	}
	Color& Color::operator-=(Color operand)
	{
		this->r -= operand.r;
		this->g -= operand.g;
		this->r -= operand.b;
		return *this;
	}
	Color Color::operator*(float operand)
	{
		Color temp = *this;
		temp.r *= operand;
		temp.g *= operand;
		temp.r *= operand;
		return temp;
	}
	Color Color::operator/(float operand)
	{
		Color temp = *this;
		temp.r /= operand;
		temp.g /= operand;
		temp.r /= operand;
		return temp;
	}
}