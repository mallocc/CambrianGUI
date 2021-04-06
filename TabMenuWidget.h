#pragma once

#include "LayoutWidget.h"

namespace gui
{
	const std::string STR_TABMENUWIDGET = "tabmenu";
	struct TabMenuWidget : public LayoutWidget
	{
		LayoutWidget* tabPane = nullptr;
		LayoutWidget* tabMenu = nullptr;
		Texture* tabSelectionTex = nullptr;
		Texture* tabOverTex = nullptr;
		nlohmann::json tabLabelTemplate;

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(std::string intentChoice);
		TabMenuWidget(GUI* gui, nlohmann::json j);
	};
}