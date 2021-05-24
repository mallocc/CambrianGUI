#pragma once

#include "Widget.h"

namespace gui
{
	class Slider : public WidgetType<Slider>, public Widget
	{
		DEFINE_WIDGET_TYPE("slider");
	public:
		Slider(GUI* gui);

		virtual void draw(float tx, float ty, bool editMode = false) override;
		virtual bool init(nlohmann::json j, bool ignoreType = false) override;
		float getValToRatio();
		float getRatioToVal(float ratio);
		virtual Widget* onKeyEvent(KeyEventData keyEventData) override;
		virtual bool onMiddleClickEvent(MouseEventData mouseEventData, bool process) override;
		virtual bool onDownEvent(MouseEventData mouseEventData, bool process) override;

		virtual bool isVertical();
		virtual bool isHorizontal();
		virtual void setVertical(bool vertical = true);
		virtual void setHorizontal();
		virtual bool& getVertical();

		std::function<void(float)> onValueChanged;

		virtual void setValue(float value);
		virtual void setInitialValue(float value);

		virtual float getValue();
		virtual float getDefaultValue();
		virtual float getInitialValue();
		virtual float getMinValue();
		virtual float getMaxValue();
		virtual bool isFloatingLabel();

	private:
		Texture* m_foreground = nullptr;
		float m_defaultVal = 0.0f;
		float m_val = 0.0f;
		float m_initialVal = 0.0f;
		float m_minVal = 0.0f;
		float m_maxVal = 1.0f;
		bool m_floatingLabel = false;
		bool m_vertical = false;
	};
}
