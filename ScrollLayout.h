#pragma once

#include "Layout.h"
#include "Slider.h"

namespace gui
{
	class ScrollLayout : public WidgetType<ScrollLayout>, public Layout
	{
		DEFINE_WIDGET_TYPE("scrolllayout");
	public:
		ScrollLayout(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
		virtual void revalidate() override;

		nlohmann::json m_vscrollbarJson;
		Slider* m_vscrollbar = nullptr;
		nlohmann::json m_hscrollbarJson;
		Slider* m_hscrollbar = nullptr;
		nlohmann::json m_contentJson;
		Widget* m_contentHolder = nullptr;
	};
}
