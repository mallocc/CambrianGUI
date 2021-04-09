#pragma once

#include "ContainerWidget.h"

namespace gui
{
	const std::string TABMENUWIDGET_CLASSNAME = "tabmenu";
	struct TabMenuWidget : public ContainerWidget
	{
		ContainerWidget* tabPane = nullptr;
		ContainerWidget* tabMenu = nullptr;
		Texture* tabSelectionTex = nullptr;
		Texture* tabOverTex = nullptr;
		nlohmann::json tabLabelTemplate;

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(std::string intentChoice);
		TabMenuWidget(GUI* gui, nlohmann::json j);
	};
}