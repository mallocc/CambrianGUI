#pragma once

#include "HLayout.h"
#include "Label.h"

namespace gui
{
	class Dropdown : public HLayout, public WidgetType<Dropdown>
	{
	public:
		DEFINE_WIDGET_TYPE("dropdown");
		Widget* icon;
		nlohmann::json iconJson;
		Label* label;
		nlohmann::json labelJson;
		nlohmann::json data;
		uint32_t defaultChoice = 0U;
		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
		virtual void onIntent(nlohmann::json intent) override;

		virtual bool onClickEvent(MouseEventData mouseEventData, bool process) override;

		virtual void openDropdown();
		virtual nlohmann::json toJson();
		Dropdown(GUI* gui);
	};
}