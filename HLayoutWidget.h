#pragma once

#include "LayoutWidget.h"

namespace gui
{
	const std::string HLAYOUTWIDGET_CLASSNAME = "hlayout";
	struct HLayoutWidget : LayoutWidget
	{
		HLayoutWidget(GUI* gui, nlohmann::json j);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
	};
}