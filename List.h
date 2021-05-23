#pragma once

#include <VLayout.h>

namespace gui
{
	class List : public WidgetType<List>, public VLayout
	{
		DEFINE_WIDGET_TYPE("list");
	public:

		List(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;

		virtual void onItemClickedEvent(Widget* clickedItem);

		std::function<void(Widget*)> onItemClicked;

		//std::string dataStr;
		nlohmann::json data;

	};
}