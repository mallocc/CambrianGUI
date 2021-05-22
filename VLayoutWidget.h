#pragma once

#include "ContainerWidget.h"

namespace gui
{
	class VLayout : public Container, public WidgetType<VLayout>
	{
		DEFINE_WIDGET_TYPE("vlayout");

		VLayout(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}