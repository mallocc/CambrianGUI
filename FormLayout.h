#pragma once

#include "VLayout.h"

namespace gui
{
	class FormLayout : public WidgetType<FormLayout>, public VLayout
	{
		DEFINE_WIDGET_TYPE("formlayout");
	public:
		FormLayout(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;

		virtual bool initList(nlohmann::json j);

		nlohmann::json data;
		nlohmann::json labelTemplate;
	};
}