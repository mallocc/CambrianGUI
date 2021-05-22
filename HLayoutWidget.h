#pragma once

#include "ContainerWidget.h"

namespace gui
{
	class HLayoutWidget : public ContainerWidget, public WidgetType<HLayoutWidget>
	{
	public:
		DEFINE_WIDGET_TYPE("hlayout");
		HLayoutWidget(GUI* gui, nlohmann::json j);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}