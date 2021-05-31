#pragma once

#include "Slider.h"

namespace gui
{
	class VSlider : public WidgetType<VSlider>, public Slider
	{
		DEFINE_WIDGET_TYPE("vslider");
	public:
		VSlider(GUI* gui);

		virtual bool init(const nlohmann::json& j, bool ignoreType = false) override;
	};
}
