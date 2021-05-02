#pragma once

#include "ContainerWidget.h"

namespace gui
{
	struct VLayoutWidget : ContainerWidget, WidgetType<VLayoutWidget>
	{
		DEFINE_WIDGET_TYPE("vlayout");

		VLayoutWidget(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}