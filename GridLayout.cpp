#include "GridLayout.h"

gui::GridLayout::GridLayout(GUI* gui) : Layout(gui)
{
}

bool gui::GridLayout::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<GridLayout>(ignoreType))
		{
		}
	}
	return true;
}
