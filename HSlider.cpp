#include "HSlider.h"

gui::HSlider::HSlider(GUI* gui) : Slider(gui)
{
}

bool gui::HSlider::init(nlohmann::json j, bool ignoreType)
{
	if (Slider::init(j, true))
	{
		if (checkWidgetType<HSlider>(ignoreType))
		{
			m_config["vertical"] = { getVertical(), "false" };
			setHorizontal();
		}
	}
	return true;
}
