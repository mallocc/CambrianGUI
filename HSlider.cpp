#include "HSlider.h"

gui::HSlider::HSlider(GUI* gui) : HLayout(gui)
{
}

bool gui::HSlider::init(nlohmann::json j, bool ignoreType)
{
	if (HLayout::init(j, true))
	{
		if (checkWidgetType<HSlider>(ignoreType))
		{
		}
	}
	return true;
}
