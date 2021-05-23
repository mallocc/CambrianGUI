#include "TextEdit.h"

gui::TextEdit::TextEdit(GUI* gui) : Label(gui)
{
}

bool gui::TextEdit::init(nlohmann::json j, bool ignoreType)
{
	if (Label::init(j, true))
	{
		if (checkWidgetType<TextEdit>(ignoreType))
		{
		}
	}
	return true;
}
