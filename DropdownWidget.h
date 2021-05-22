#pragma once

#include "HLayoutWidget.h"
#include "LabelWidget.h"

namespace gui
{
	class DropdownWidget : public HLayoutWidget, public WidgetType<DropdownWidget>
	{
	public:
		DEFINE_WIDGET_TYPE("dropdown");
		Widget* icon;
		nlohmann::json iconJson;
		LabelWidget* label;
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