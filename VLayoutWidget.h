#pragma once

#include "LayoutWidget.h"

namespace gui
{
	const std::string STR_VLAYOUTWIDGET = "vlayout";
	struct VLayoutWidget : LayoutWidget
	{
		VLayoutWidget(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}