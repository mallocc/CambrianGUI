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
		virtual Widget* onKeyEvent(KeyEventData keyEventData) override;

		std::function<void(float)> onValueChanged;


		virtual bool onClickEvent(MouseEventData mouseEventData, bool process) override;
		virtual bool onDoubleClickEvent(MouseEventData mouseEventData, bool process) override;
		virtual bool onDragEvent(MouseEventData mouseEventData, bool process) override;

		std::string roundNumber(float number);

	private:
		float m_minVal = 0;
		float m_maxVal = 128;
		float m_defaultVal = m_minVal;
		float m_val = m_defaultVal;
		float m_initialVal = 0.0f;
	};
}
