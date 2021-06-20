#include "Slider.h"
#include <GL/glew.h>
#include "GUI.h"

gui::Slider::Slider(GUI* gui) : Layout(gui)
{
	onValueChanged = [](float) {};
}

inline std::string gui::Slider::roundNumber(float number)
{
	std::stringstream ss;
	ss.precision(3);
	ss << number;
	return ss.str();
}

void gui::Slider::revalidateSlider(MouseEventData mouseEventData)
{
	if (m_slider != nullptr)
	{
		float tx = 0, ty = 0;
		getAbsolutePosition(tx, ty);

		showFloatingLabel(mouseEventData);

		if (m_vertical)
		{
			//setValue(getRatioToVal(1.0f - (mouseEventData.y - ty - m_slider->H() / 2.0f) / (float)(H() - m_slider->H())));
			setValue(getRatioToVal((mouseEventData.y - ty - m_slider->H() / 2.0f) / (float)(H() - m_slider->H())));
			setValue(std::min(m_maxVal, std::max(m_minVal, getValue())));
		}
		else
		{
			setValue(getRatioToVal(mouseEventData.x - tx - m_slider->W() / 2.0f) / (float)(W() - m_slider->W()));
			setValue(std::min(m_maxVal, std::max(m_minVal, getValue())));
		}

		setSliderPosition();

		onValueChanged(getValue());
	}
}

void gui::Slider::showFloatingLabel(MouseEventData mouseEventData)
{
	if (m_floatingLabel)
	{
		if (m_vertical)
		{
			getGUI()->showFloatingLabel(
				mouseEventData.x + 10,
				mouseEventData.y - getGUI()->getWidgetManager()->getFloatingLabelWidget()->H() / 2.0f,
				roundNumber(getValue()), 50);
		}
		else
		{
			getGUI()->showFloatingLabel(
				mouseEventData.x + 10,
				mouseEventData.y - getGUI()->getWidgetManager()->getFloatingLabelWidget()->H() / 2.0f,
				roundNumber(getValue()), 50);
		}
	}
}

void gui::Slider::setSliderPosition()
{
	if (m_vertical)
	{
		m_slider->setX(0, FORCE);
		//m_slider->setY((1.0f - getValToRatio()) * (H() - m_slider->H()), FORCE);
		m_slider->setY((getValToRatio()) * (H() - m_slider->H()), FORCE);
	}
	else
	{
		m_slider->setX(getValToRatio() * (W() - m_slider->W()), FORCE);
		m_slider->setY(0, FORCE);
	}
}

gui::Widget* gui::Slider::getSlider()
{
	return m_slider;
}

bool gui::Slider::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<Slider>(ignoreType))
		{
			ConfigList fields;
			{
				fields["slider"] = m_sliderJson;
				fields["value"] = { 
					"0.0", 
					[&](std::string value) {
						setValue(std::atof(value.c_str()));
					},
					[&](std::string value) {
						return nlohmann::json({ {value, getValue() } });
					}					 
				};
				fields["default"] = { m_defaultVal, "0.0" };
				fields["min"] = { m_minVal, "0.0" };
				fields["max"] = { m_maxVal, "1.0" };
				fields["label"] = m_floatingLabel;
				fields["vertical"] = m_vertical;
			}
			fields.load(j);
			m_config += fields;

			m_initialVal = getValue();

			create_widget(sliderWidget, m_sliderJson)
			{
				m_slider = sliderWidget;
				m_slider->onDown = [=](GUI* gui, MouseEventData mouseEventData)
				{		
					revalidateSlider(mouseEventData);
				};
				addChild(sliderWidget);
			}

			setSliderPosition();
		}
	}
	return true;
}

float gui::Slider::getValToRatio()
{
	return (getValue() - m_minVal) / std::fabs(m_maxVal - m_minVal);
}

float gui::Slider::getRatioToVal(float ratio)
{
	return ratio * std::fabs(m_maxVal - m_minVal) + m_minVal;
}

bool gui::Slider::onMiddleClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Layout::onMiddleClickEvent(mouseEventData, process);
	if (handled && process)
	{
		setValue(m_defaultVal);
		setSliderPosition();
		onValueChanged(getValue());
	}
	return handled;
}

bool gui::Slider::onClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Layout::onClickEvent(mouseEventData, process);
	if (handled && process)
	{
		//revalidateSlider(mouseEventData);
	}
	return handled;
}

bool gui::Slider::isVertical()
{
	return m_vertical;
}

bool gui::Slider::isHorizontal()
{
	return !m_vertical;
}

void gui::Slider::setVertical(bool vertical)
{
	this->m_vertical = vertical;
}

void gui::Slider::setHorizontal()
{
	this->m_vertical = false;
}

bool& gui::Slider::getVertical()
{
	return m_vertical;
}

void gui::Slider::setValue(float value)
{
	this->m_val = value;
	if (this->m_valRef != nullptr)
		*this->m_valRef = value;
}

void gui::Slider::setValueRef(float* valueRef)
{
	this->m_valRef = valueRef;
}

void gui::Slider::setInitialValue(float value)
{
	this->m_initialVal = value;
}

float gui::Slider::getValue()
{
	if (this->m_valRef != nullptr)
		return *this->m_valRef;
	return m_val;
}

float gui::Slider::getDefaultValue()
{
	return m_defaultVal;
}

float gui::Slider::getInitialValue()
{
	return m_initialVal;
}

float gui::Slider::getMinValue()
{
	return m_minVal;
}

float gui::Slider::getMaxValue()
{
	return m_maxVal;
}

bool gui::Slider::isFloatingLabel()
{
	return m_floatingLabel;
}
