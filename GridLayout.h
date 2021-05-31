#pragma once

#include "Layout.h"

namespace gui
{
	class GridLayout : public WidgetType<GridLayout>, public Layout
	{
		DEFINE_WIDGET_TYPE("gridlayout");
	public:
		GridLayout(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}
