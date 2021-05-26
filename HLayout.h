#pragma once

#include "Layout.h"

namespace gui
{
	class HLayout : public Layout, public WidgetType<HLayout>
	{
		DEFINE_WIDGET_TYPE("hlayout");
	public:
		HLayout(GUI* gui);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
		virtual float getPreferedHeight(Widget* child);
	};
}