#include "ScrollLayout.h"

gui::ScrollLayout::ScrollLayout(GUI* gui) : Layout(gui)
{
}

bool gui::ScrollLayout::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<ScrollLayout>(ignoreType))
		{
		}
	}
	return true;
}
