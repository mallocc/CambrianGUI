#pragma once

#include "Widget.h"

namespace gui
{
	class SwitchWidget : public Widget, public WidgetType<SwitchWidget>
	{
	public:
		DEFINE_WIDGET_TYPE("switch");

		Texture* switchOnTex = nullptr;
		Texture* switchOffTex = nullptr;
		Texture* currentSwitchTex = nullptr;

		float activationValue = 0.5f;
		float currentValue = 0.5f;
		bool radio = false;

		virtual void draw(float tx, float ty, bool editMode = false);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate() override;

		virtual void toggleSwitch();
		virtual void switchOn();
		virtual void switchOff();

		SwitchWidget(GUI* gui, nlohmann::json j);
	};
}