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
		if (doesTypeMatch(ignoreType))
		{
			addToManifestList(j,
				{
					{
						"switch-off",
						{"",
						[&](std::string value) { switchOffTex = gui->getTextureManager()->requireTexture(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, switchOffTex == nullptr ? "" : switchOffTex->name }}); }}
					},
					{
						"switch-on",
						{"",
						[&](std::string value) { switchOnTex = gui->getTextureManager()->requireTexture(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, switchOnTex == nullptr ? "" : switchOnTex->name }}); }}
					},
					{
						"activation-value",
						{"0.5",
						[&](std::string value) { activationValue = std::atof(value.c_str()); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, activationValue }}); }}
					},
					{
						"radio",
						{"false",
						[&](std::string value) { radio = "true" == value; },
						[&](std::string fieldName) { return nlohmann::json({{fieldName, radio}}); }}
					}
				}
			);
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
	setClassname(STR_SWITCHWIDGET);
	onRelease = [&](GUI* gui, MouseEventData mouseEventData)
	{
		toggleSwitch();
		radioUp();
	};
	init(j);
	currentSwitchTex = switchOffTex;
}