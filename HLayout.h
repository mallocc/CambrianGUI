#pragma once

#include "Layout.h"

namespace gui
{
	class HLayout : public Layout, public WidgetType<HLayout>
	{
		DEFINE_WIDGET_TYPE("hlayout");
	public:
		HLayout(GUI* gui);
		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
		virtual void revalidate() override;

		virtual float getPreferedWidth(Widget* child) override;
	};
}