#pragma once

#include "ContainerWidget.h"

namespace gui
{
	struct TabMenuWidget : ContainerWidget, WidgetType<TabMenuWidget>
	{
		DEFINE_WIDGET_TYPE("tabmenu");

		ContainerWidget* tabPane = nullptr;
		ContainerWidget* tabMenu = nullptr;
		Texture* tabSelectionTex = nullptr;
		Texture* tabOverTex = nullptr;
		nlohmann::json tabLabelTemplate;

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);
		TabMenuWidget(GUI* gui, nlohmann::json j);
	};
}