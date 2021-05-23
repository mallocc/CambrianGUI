#pragma once

#include "VLayout.h"
#include "Label.h"

namespace gui
{
	class DropdownList : public VLayout, public WidgetType<DropdownList>
	{
	public:
		DEFINE_WIDGET_TYPE("dropdownlist");
		std::map<std::string, Label*> data;
		nlohmann::json labelTemplate;
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual bool initList(nlohmann::json j);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual void onIntent(nlohmann::json intent = nlohmann::json());
		DropdownList(GUI* gui);

		bool floating = false;
	};
}