#include <GL/glew.h>
#include "SwitchWidget.h"
#include "GUI.h"

using gui::Widget;

void gui::SwitchWidget::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);
	tx += x; ty += y;

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
		glScalef(w, h, 1);
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

bool gui::SwitchWidget::init(nlohmann::json j, bool ignoreType)
{
	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<SwitchWidget>(ignoreType))
		{
			config["switch-off"] = textureConfigItem(switchOffTex);
			config["switch-on"] = textureConfigItem(switchOnTex);
			config["activation-value"] = activationValue;
			config.load(j);

			if (currentValue > activationValue)
				check();
			else
				uncheck();
		}
	}

	return true;
}

void gui::SwitchWidget::revalidate()
{
	Widget::revalidate();

	if (scaled)
	{
		if (switchOffTex != nullptr)
		{
			w = switchOffTex->width;
			h = switchOffTex->height;
		}
	}
}

void gui::SwitchWidget::toggleSwitch()
{
	currentValue = currentValue <= activationValue ? 1.0f : 0.0f;
}

void gui::SwitchWidget::switchOn()
{
	currentValue = 1.0f;
}

void gui::SwitchWidget::switchOff()
{
	currentValue = 0.0f;
}


gui::SwitchWidget::SwitchWidget(GUI* gui, nlohmann::json j) : Widget(gui)
{
	onRelease = [&](GUI* gui, MouseEventData mouseEventData)
	{
		toggleSwitch();
		radioUp();
	};
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
	currentSwitchTex = switchOffTex;
}