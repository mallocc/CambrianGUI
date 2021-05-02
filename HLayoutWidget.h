#pragma once

#include "ContainerWidget.h"

namespace gui
{
	struct HLayoutWidget : ContainerWidget, WidgetBase<HLayoutWidget>
	{
		static std::string getClassname()
		{
			return "hlayout";
		}

		HLayoutWidget(GUI* gui, nlohmann::json j);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}