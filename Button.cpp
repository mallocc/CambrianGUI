#include "Button.h"

gui::Button::Button(GUI* gui) : Widget(gui)
{
}

bool gui::Button::init(const nlohmann::json& j, bool ignoreType)
{
	if (Widget::init(j, true))
	{
		if (checkWidgetType<Button>(ignoreType))
		{
		}
	}
	return true;
}
