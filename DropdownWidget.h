#pragma once

#include "HLayoutWidget.h"

namespace gui
{
	const std::string STR_DROPDOWNWIDGET = "dropdown";
	struct DropdownWidget : HLayoutWidget
	{
		Widget* icon;
		nlohmann::json iconJson;
		Widget* label;
		nlohmann::json labelJson;
		nlohmann::json data;
		uint32_t defaultChoice = 0U;
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(std::string intentChoice);
		virtual void openDropdown();
		virtual nlohmann::json toJson();
		DropdownWidget(GUI* gui, nlohmann::json j);
	};
}