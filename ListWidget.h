#pragma once

#include <VLayoutWidget.h>

namespace gui
{
	class ListWidget : public VLayoutWidget, public WidgetType<ListWidget>
	{
	public:
		DEFINE_WIDGET_TYPE("list");

		ListWidget(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);

		//std::string dataStr;
		nlohmann::json data;

	};
}