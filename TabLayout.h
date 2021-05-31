#pragma once

#include "Layout.h"

namespace gui
{
	class TabLayout : public WidgetType<TabLayout>, public Layout
	{
		DEFINE_WIDGET_TYPE("tablayout");
	public:
		TabLayout(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}
