#include <GL/glew.h>
#include "Switch.h"
#include "GUI.h"

using gui::Widget;

void gui::Switch::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);
	tx += X(); ty += Y();

	currentSwitchTex = currentValue > activationValue ? switchOnTex : switchOffTex;

	if (currentSwitchTex != nullptr)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, currentSwitchTex->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glColor4f(1, 1, 1, 1.0f);
		glPushMatrix();
		glTranslatef(tx, ty, 0);
		glScalef(W(), H(), 1);
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
	}
}

bool gui::Switch::init(nlohmann::json j, bool ignoreType)
{
	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<Switch>(ignoreType))
		{
			ConfigList fields;
			{
				fields["switch-off"] = textureConfigItem(switchOffTex);
				fields["switch-on"] = textureConfigItem(switchOnTex);
				fields["activation-value"] = activationValue;
				fields.load(j);
			}

			m_config += fields;

			if (currentValue > activationValue)
				check();
			else
				uncheck();
		}
	}

	return true;
}

void gui::Switch::revalidate()
{
	Widget::revalidate();

	if (isScaled())
	{
		if (switchOffTex != nullptr)
		{
			setW(switchOffTex->width, FORCE);
			setH(switchOffTex->height, FORCE);
		}
	}
}

void gui::Switch::toggleSwitch()
{
	currentValue = currentValue <= activationValue ? 1.0f : 0.0f;
}

void gui::Switch::switchOn()
{
	currentValue = 1.0f;
}

void gui::Switch::switchOff()
{
	currentValue = 0.0f;
}


gui::Switch::Switch(GUI* gui) : Widget(gui)
{
	onRelease = [&](GUI* gui, MouseEventData mouseEventData)
	{
		toggleSwitch();
		//radioUp();
	};
	currentSwitchTex = switchOffTex;
}