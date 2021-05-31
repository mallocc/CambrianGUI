#pragma once

#include "Widget.h"

namespace gui
{
	class Button : public WidgetType<Button>, public Widget
	{
		DEFINE_WIDGET_TYPE("button");
	public:
		Button(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}