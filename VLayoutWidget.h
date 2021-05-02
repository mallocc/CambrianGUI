#pragma once

#include "ContainerWidget.h"

namespace gui
{
	struct VLayoutWidget : ContainerWidget, WidgetBase<VLayoutWidget>
	{
		static std::string getClassname()
		{
			return "vlayout";
		}
		VLayoutWidget(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}