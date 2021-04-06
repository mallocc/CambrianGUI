#pragma once

#include <inttypes.h>

namespace gui
{
	struct MouseEventData
	{
		int32_t x = 0;
		int32_t y = 0;
		bool leftDown = false;
		bool rightDown = false;
		bool middleDown = false;
	};

	struct KeyEventData
	{
		bool shiftDown = false;
		bool tabDown = false;
		bool ctrlDown = false;
		unsigned char keyboardState[256];
	};
}
