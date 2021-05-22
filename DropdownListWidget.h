#pragma once

#include "VLayoutWidget.h"
#include "LabelWidget.h"

namespace gui
{
	class DropdownListWidget : public VLayoutWidget, public WidgetType<DropdownListWidget>
	{
	public:
		DEFINE_WIDGET_TYPE("dropdownlist");
		std::map<std::string, LabelWidget*> data;
		nlohmann::json labelTemplate;
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual bool initList(nlohmann::json j);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual void onIntent(nlohmann::json intent = nlohmann::json());
		DropdownListWidget(GUI* gui, nlohmann::json j);

		bool floating = false;
	};
}