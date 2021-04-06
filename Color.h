#pragma once

#include "Utilities.h"

namespace gui
{
	struct Color
	{
		float r, g, b;
	};

	static gui::Color hex2rgb(std::string hex)
	{
		Color color = { 0, 0, 0 };

		if (!hex.empty())
		{
			if (hex.at(0) == '#')
			{
				hex.erase(0, 1);
			}

			while (hex.length() != 6)
			{
				hex += "0";
			}

			std::vector<std::string> colori = splitWithCharacters(hex, 2);

			color.r = stoi(colori[0], nullptr, 16) / (float)255;
			color.g = stoi(colori[1], nullptr, 16) / (float)255;
			color.b = stoi(colori[2], nullptr, 16) / (float)255;
		}

		return color;
	}
}