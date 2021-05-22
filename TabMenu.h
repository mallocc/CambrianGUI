#pragma once

#include "Container.h"

namespace gui
{
	class TabMenu : public Container, public WidgetType<TabMenu>
	{
	public:
		DEFINE_WIDGET_TYPE("tabmenu");

		Container* tabPane = nullptr;
		Container* tabMenu = nullptr;
		Texture* tabSelectionTex = nullptr;
		Texture* tabOverTex = nullptr;
		nlohmann::json tabLabelTemplate;

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);
		TabMenu(GUI* gui, nlohmann::json j);
	};
}