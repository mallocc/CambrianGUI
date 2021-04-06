#pragma once

#include "VLayoutWidget.h"
#include "LabelWidget.h"

namespace gui
{
	const std::string STR_DROPDOWNLISTWIDGET = "dropdownlist";
	struct DropdownListWidget : VLayoutWidget
	{
		std::map<std::string, LabelWidget*> data;
		nlohmann::json labelTemplate;
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual bool initList(nlohmann::json j);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual void onIntent(std::string intentChoice);
		DropdownListWidget(GUI* gui, nlohmann::json j);
	};
}