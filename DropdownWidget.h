#pragma once

#include "HLayoutWidget.h"

namespace gui
{
	struct DropdownWidget : HLayoutWidget, WidgetType<DropdownWidget>
	{
		DEFINE_WIDGET_TYPE("dropdown");
		Widget* icon;
		nlohmann::json iconJson;
		Widget* label;
		nlohmann::json labelJson;
		nlohmann::json data;
		uint32_t defaultChoice = 0U;
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);
		virtual void openDropdown();
		virtual nlohmann::json toJson();
		DropdownWidget(GUI* gui, nlohmann::json j);
	};
}