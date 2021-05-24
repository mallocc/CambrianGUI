#include "Slider.h"
#include <GL/glew.h>
#include "GUI.h"

gui::Slider::Slider(GUI* gui) : Widget(gui)
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

bool gui::Slider::init(nlohmann::json j, bool ignoreType)
{
	if (Widget::init(j, true))
	{
		if (checkWidgetType<Slider>(ignoreType))
		{
			ConfigList fields;
			{
				fields["foreground"] = textureConfigItem(m_foreground);
				fields["value"] = { m_val, "0.0" };
				fields["default"] = { m_defaultVal, "0.0" };
				fields["min"] = { m_minVal, "0.0" };
				fields["max"] = { m_maxVal, "1.0" };
				fields["label"] = m_floatingLabel;
				fields["vertical"] = m_vertical;
			}
			fields.load(j);
			m_config += fields;

			m_initialVal = m_val;
		}
	}
	return true;
}

float gui::Slider::getValToRatio()
{
	return (m_val - m_minVal) / std::fabs(m_maxVal - m_minVal);
}

float gui::Slider::getRatioToVal(float ratio)
{
	return ratio * std::fabs(m_maxVal - m_minVal) + m_minVal;
}

void gui::Slider::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);
	tx += X(); ty += Y();

	if (m_foreground != nullptr)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, m_foreground->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glColor3f(1, 1, 1);
		glPushMatrix();
		if (m_vertical)
		{
			glTranslatef(tx, ty + (1.0f - getValToRatio()) * (H() - W()), 0);
			glScalef(W(), W(), 1);
		}
		else
		{
			glTranslatef(tx + getValToRatio() * (W() - H()), ty, 0);
			glScalef(H(), H(), 1);
		}
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(1, 0);
			glVertex2f(1, 0);
			glTexCoord2f(1, 1);
			glVertex2f(1, 1);
			glTexCoord2f(0, 1);
			glVertex2f(0, 1);
		}
		glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, NULL);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

gui::Widget* gui::Slider::onKeyEvent(KeyEventData keyEventData)
{
	if (keyUp(VK_LSHIFT, &keyEventData) || keyDown(VK_LSHIFT, &keyEventData))
	{
		this->m_initialVal = m_val;
		setInitialMouseEventData(getMouseEventData());
	}

	Widget::onKeyEvent(keyEventData);
	return nullptr;
}

bool gui::Slider::onMiddleClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Widget::onMiddleClickEvent(mouseEventData, process);
	if (handled && process)
	{
		m_val = m_defaultVal;
		onValueChanged(m_val);
	}
	return handled;
}

bool gui::Slider::onDownEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = Widget::onDownEvent(mouseEventData, process);
	if (handled && process)
	{
		float tx = 0, ty = 0;
		getAbsolutePosition(tx, ty);

		if (m_vertical)
		{
			m_val = getRatioToVal(1.0f - (mouseEventData.y - ty - W() / 2.0f) / (float)(H() - W()));
			m_val = std::min(m_maxVal, std::max(m_minVal, m_val));
		}
		else
		{
			m_val = (mouseEventData.x - tx - H() / 2.0f) / (float)(W() - H());
			m_val = std::min(1.0f, std::max(0.0f, m_val));
		}

		onValueChanged(m_val);

		if (m_floatingLabel)
		{
			if (m_vertical)
			{
				getGUI()->showFloatingLabel(
					tx + W(), 
					ty + (1.0f - getValToRatio()) * (H() - W()), 
					roundNumber(m_val), 100);
			}
			else
			{
				getGUI()->showFloatingLabel(
					tx 
					+ getValToRatio() * (W() - H()) 
					- getGUI()->getWidgetManager()->getFloatingLabelWidget()->W() / 2.0 
					+ H() / 2.0f, 
					ty - getGUI()->getWidgetManager()->getFloatingLabelWidget()->H(),
					roundNumber(m_val), 100);
			}
		}
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
}

void gui::Slider::setInitialValue(float value)
{
	this->m_initialVal = value;
}

float gui::Slider::getValue()
{
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
