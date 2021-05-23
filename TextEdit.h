#pragma once

#include "Label.h"

namespace gui
{
	class TextEdit : public WidgetType<TextEdit>, public Label
	{
		DEFINE_WIDGET_TYPE("textedit");
	public:
		TextEdit(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
	};
}