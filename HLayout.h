#pragma once

#include "Layout.h"

namespace gui
{
	class HLayout : public Layout, public WidgetType<HLayout>
	{
	public:
		DEFINE_WIDGET_TYPE("hlayout");
		HLayout(GUI* gui);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}