#include "VSlider.h"

gui::VSlider::VSlider(GUI* gui) : VLayout(gui)
{
}

bool gui::VSlider::init(nlohmann::json j, bool ignoreType)
{
	if (VLayout::init(j, true))
	{
		if (checkWidgetType<VSlider>(ignoreType))
		{
		}
	}
	return true;
}
