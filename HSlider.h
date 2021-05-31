#pragma once

#include "Slider.h"

namespace gui
{
	class HSlider : public WidgetType<HSlider>, public Slider
	{
		DEFINE_WIDGET_TYPE("hslider");
	public:
		HSlider(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}
