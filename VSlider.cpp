#include "VSlider.h"

gui::VSlider::VSlider(GUI* gui) : Slider(gui)
{
}

bool gui::VSlider::init(nlohmann::json j, bool ignoreType)
{
	if (Slider::init(j, true))
	{
		if (checkWidgetType<VSlider>(ignoreType))
		{
			m_config["vertical"] = { getVertical(), "true" };
			setVertical();
		}
	}
	return true;
}
