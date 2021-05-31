#include "TabLayout.h"

gui::TabLayout::TabLayout(GUI* gui) : Layout(gui)
{
}

bool gui::TabLayout::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<TabLayout>(ignoreType))
		{
		}
	}
	return true;
}
