#pragma once

#include "VLayout.h"

namespace gui
{
	class VSlider : public WidgetType<VSlider>, public VLayout
	{
		DEFINE_WIDGET_TYPE("vslider");
	public:
		VSlider(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
	};
}
