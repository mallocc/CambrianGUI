#pragma once

#include "HLayout.h"
#include "Label.h"

namespace gui
{
	class Dropdown : public WidgetType<Dropdown>, public HLayout
	{
		DEFINE_WIDGET_TYPE("dropdown");
	public:
		Widget* icon = nullptr;
		nlohmann::json iconJson;
		Label* label = nullptr;
		nlohmann::json labelJson;
		nlohmann::json data;
		uint32_t defaultChoice = 0U;
		virtual bool init(nlohmann::json j, bool ignoreType = false) override;

		virtual bool onClickEvent(MouseEventData mouseEventData, bool process) override;


		virtual void onSelectionEvent(nlohmann::json data);

		intentcallback_t onSelection;

		virtual void openDropdown();
		virtual void closeDropdown();
		virtual nlohmann::json toJson();
		Dropdown(GUI* gui);
	};
}