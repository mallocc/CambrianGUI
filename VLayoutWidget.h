#pragma once

#include "ContainerWidget.h"

namespace gui
{
	const std::string VLAYOUTWIDGET_CLASSNAME = "vlayout";
	struct VLayoutWidget : ContainerWidget
	{
		VLayoutWidget(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}