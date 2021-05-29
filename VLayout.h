#pragma once

#include "Layout.h"

namespace gui
{
	class VLayout : public Layout, public WidgetType<VLayout>
	{
		DEFINE_WIDGET_TYPE("vlayout");
	public:

		VLayout(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
		virtual void revalidate() override;

		virtual float getPreferedHeight(Widget* child) override;
	};
}