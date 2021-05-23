#pragma once

#include "Layout.h"

namespace gui
{
	class TabMenu : public Layout, public WidgetType<TabMenu>
	{
	public:
		DEFINE_WIDGET_TYPE("tabmenu");

		Layout* tabPane = nullptr;
		Layout* tabMenu = nullptr;
		Texture* tabSelectionTex = nullptr;
		Texture* tabOverTex = nullptr;
		nlohmann::json tabLabelTemplate;

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);
		TabMenu(GUI* gui);
	};
}