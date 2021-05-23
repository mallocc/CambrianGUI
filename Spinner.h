#pragma once

#include "Label.h"

namespace gui
{
	class Spinner : public WidgetType<Spinner>, public Label
	{
		DEFINE_WIDGET_TYPE("spinner");
	public:
		Spinner(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
	};
}
