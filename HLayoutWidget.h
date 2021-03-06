#pragma once

#include "ContainerWidget.h"

namespace gui
{
	struct HLayoutWidget : ContainerWidget, WidgetType<HLayoutWidget>
	{
		DEFINE_WIDGET_TYPE("hlayout");
		HLayoutWidget(GUI* gui, nlohmann::json j);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}