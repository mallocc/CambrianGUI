#include "Spinner.h"

gui::Spinner::Spinner(GUI* gui): Label(gui)
{
}

bool gui::Spinner::init(nlohmann::json j, bool ignoreType)
{
	if (Label::init(j, true))
	{
		if (checkWidgetType<Spinner>(ignoreType))
		{
		}
	}
	return true;
}
