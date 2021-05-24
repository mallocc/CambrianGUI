#include "Spinner.h"
#include "GUI.h"
#include <sstream>

gui::Spinner::Spinner(GUI* gui): Label(gui)
{
	onValueChanged = [](float) {};
}

inline std::string gui::Spinner::roundNumber(float number)
{
	std::stringstream ss;
	ss.precision(3);
	ss << number;
	return ss.str();
}

gui::Widget* gui::Spinner::onKeyEvent(KeyEventData keyEventData)
{
	if (keyDown(VK_LSHIFT, &keyEventData) || keyUp(VK_LSHIFT, &keyEventData))
	{
		this->m_initialVal = this->m_val;
		setInitialMouseEventData(getMouseEventData());
	}

	Label::onKeyEvent(keyEventData);
	return nullptr;
}

bool gui::Spinner::onClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Label::onClickEvent(mouseEventData, process);
	if (handled && process)
	{
		this->m_initialVal = this->m_val;
		setInitialMouseEventData(getMouseEventData());
	}
	return handled;
}

bool gui::Spinner::onDoubleClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Label::onDoubleClickEvent(mouseEventData, process);
	if (handled && process)
	{
		this->m_val = this->m_defaultVal;
		setInitialMouseEventData(getMouseEventData());
		text = roundNumber(m_val);
		onValueChanged(this->m_val);
	}
	return handled;
}

bool gui::Spinner::onDragEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Label::onDragEvent(mouseEventData, process);
	if (handled && process)
	{
		float dy = (getInitialMouseEventData().y - mouseEventData.y) / (float)getGUI()->h * std::fabs(m_maxVal - m_minVal);
		this->m_val =
			std::max(m_minVal,
				std::min(m_maxVal,
					(dy + m_initialVal)));
		text = roundNumber(m_val);
		onValueChanged(this->m_val);
	}
	return handled;
}

bool gui::Spinner::init(nlohmann::json j, bool ignoreType)
{
	if (Label::init(j, true))
	{
		if (checkWidgetType<Spinner>(ignoreType))
		{
			ConfigList fields;
			{
				fields["min"] = m_minVal;
				fields["max"] = { m_maxVal, "128" };
				fields["default"] = m_defaultVal;
				fields.load(j);
			}
			m_config += fields;
			m_val = m_defaultVal;
			text = roundNumber(m_val);
			onValueChanged(this->m_val);
		}
	}
	return true;
}

