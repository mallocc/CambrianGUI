#pragma once

#include "Widget.h"

namespace gui
{
	define_widget_class(Switch, Widget)
	{
	public:
		DEFINE_WIDGET_TYPE("switch");

		Texture* switchOnTex = nullptr;
		Texture* switchOffTex = nullptr;
		Texture* currentSwitchTex = nullptr;

		float activationValue = 0.5f;
		float currentValue = 0.5f;
		bool radio = false;

		virtual void draw(float tx, float ty, bool editMode = false) override;
		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
		virtual void revalidate() override;


		virtual bool onReleaseEvent(MouseEventData mouseEventData, bool process) override;

		virtual void toggleSwitch();
		virtual void switchOn();
		virtual void switchOff();

		Switch(GUI* gui);
	};
}