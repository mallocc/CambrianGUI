#pragma once

#include "ContainerWidget.h"

namespace gui
{
	class HLayout : public Container, public WidgetType<HLayout>
	{
	public:
		DEFINE_WIDGET_TYPE("hlayout");
		HLayout(GUI* gui, nlohmann::json j);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}