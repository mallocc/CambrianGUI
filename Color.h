#pragma once

#include "Utilities.h"

namespace gui
{
	struct Color
	{
		float r, g, b, a;

		Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

		Color operator+(Color operand);
		Color& operator+=(Color operand);
		Color operator-(Color operand);
		Color& operator-=(Color operand);
		Color operator*(float operand);
		Color operator/(float operand);
	};

	static void lerpColor(Color& target, Color& current, const float factor)
	{
		current.r += (target.r - current.r) * factor;
		current.g += (target.g - current.g) * factor;
		current.b += (target.b - current.b) * factor;
		current.a += (target.a - current.a) * factor;
	};

	static gui::Color hex2Color(std::string hex)
	{
		Color color = { 0, 0, 0 };

		if (!hex.empty())
		{
			if (hex.at(0) == '#')
			{
				hex.erase(0, 1);
			}
			
			if (hex.length() == 3)
			{
				std::vector<std::string> colori = splitWithCharacters(hex, 1);

				color.r = stoi(colori[0], nullptr, 16) / (float)16;
				color.g = stoi(colori[1], nullptr, 16) / (float)16;
				color.b = stoi(colori[2], nullptr, 16) / (float)16;
			}
			else if (hex.length() == 4)
			{
				std::vector<std::string> colori = splitWithCharacters(hex, 1);

				color.r = stoi(colori[0], nullptr, 16) / (float)16;
				color.g = stoi(colori[1], nullptr, 16) / (float)16;
				color.b = stoi(colori[2], nullptr, 16) / (float)16;
				color.a = stoi(colori[3], nullptr, 16) / (float)16;
			}
			else if (hex.length() == 6)
			{
				std::vector<std::string> colori = splitWithCharacters(hex, 2);

				color.r = stoi(colori[0], nullptr, 16) / (float)255;
				color.g = stoi(colori[1], nullptr, 16) / (float)255;
				color.b = stoi(colori[2], nullptr, 16) / (float)255;
			}
			else if (hex.length() == 8)
			{
				std::vector<std::string> colori = splitWithCharacters(hex, 2);

				color.r = stoi(colori[0], nullptr, 16) / (float)255;
				color.g = stoi(colori[1], nullptr, 16) / (float)255;
				color.b = stoi(colori[2], nullptr, 16) / (float)255;
				color.a = stoi(colori[3], nullptr, 16) / (float)255;
			}
		}

		return color;
	}

	extern Color black;
	extern Color off_black;
	extern Color white;
	extern Color off_white;
	extern Color red;
	extern Color green;
	extern Color blue;
	extern Color cyan;
	extern Color magenta;
	extern Color yellow;
	extern Color grey;
}