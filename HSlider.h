#pragma once

#include "HLayout.h"

namespace gui
{
	class HSlider : public WidgetType<HSlider>, public HLayout
	{
		DEFINE_WIDGET_TYPE("hslider");
	public:
		HSlider(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
	};
}
