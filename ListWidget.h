#pragma once

#include <VLayoutWidget.h>

namespace gui
{
	class List : public VLayout, public WidgetType<List>
	{
	public:
		DEFINE_WIDGET_TYPE("list");

		List(GUI* gui, nlohmann::json j);

		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void onIntent(nlohmann::json intent);

		//std::string dataStr;
		nlohmann::json data;

	};
}