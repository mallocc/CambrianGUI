#pragma once

#include "Layout.h"

namespace gui
{
	class ScrollLayout : public WidgetType<ScrollLayout>, public Layout
	{
		DEFINE_WIDGET_TYPE("scrolllayout");
	public:
		ScrollLayout(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}
