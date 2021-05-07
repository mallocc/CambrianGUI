#include <GL/glew.h>
#include "LabelWidget.h"
#include "GUI.h"

void gui::LabelWidget::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);

	tx += x; ty += y;

	// Draw title
	glColor4f(textColor.r, textColor.g, textColor.b, opacity);

	auto font = displayFont;

	glPushMatrix();
	glTranslatef(std::round(tx), std::round(ty), 0);
	// centered
	if (center)
	{
		font->renderText(
			text,
			xOffset + w / 2.0f,
			yOffset + h / 2.0f - font->size / 2.0f + (font->size - 2.0f), 1., bold, italic, 1);
	}
	else
	{
		font->renderText(
			text,
			xOffset + padding,
			yOffset + h / 2.0f - font->size / 2.0f + (font->size - 2.0f), 1., bold, italic, 0);
	}
	glPopMatrix();

	if (editMode)
	{
		glColor3f(0, 0, 1);

		glBegin(GL_LINES);
		{
			glVertex2f(tx, 0);
			glVertex2f(tx, gui->h);

			glVertex2f(0, ty);
			glVertex2f(gui->w, ty);
		}
		glEnd();
	}
	glColor4f(1, 1, 1, 1);
}

bool gui::LabelWidget::init(nlohmann::json j, bool ignoreType)
{
	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<LabelWidget>(ignoreType))
		{
	/*		addConfigItem("text-color", targetTextColor);*/

			addToManifestList(j,
				{
					{
						"text-color",
						{"#ffffff",
						[&](std::string value) { gui->getGUIConfig()->getColor(value, targetTextColor); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(textColor) }}); }}
					},
					{
						"font",
						{"OpenSans@24",
						[&](std::string value) { displayFont = gui->getFontManager()->fonts[value] == nullptr ? gui->getFontManager()->defaultFont : gui->getFontManager()->fonts[value]; },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, displayFont->fontName }}); }}
					},
					{
						"revalidate-size",
						{"false",
						[&](std::string value) { revalidateSize = "true" == value; },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, revalidateSize }}); }}
					},
					{
						"center",
						{"false",
						[&](std::string value) { center = "true" == value; },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, center }}); }}
					},
					{
						"bold",
						{"false",
						[&](std::string value) { bold = "true" == value; },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, bold }}); }}
					},
					{
						"italic",
						{"false",
						[&](std::string value) { italic = "true" == value; },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, italic }}); }}
					},
					{
						"padding",
						{"0",
						[&](std::string value) { padding = std::atoi(value.c_str()); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, padding }}); }}
					},
					{
						"text-color-start",
						{"#ffffff",
						[&](std::string value) { gui->getGUIConfig()->getColor(value, textColorStart); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(textColorStart) }}); }}
					},
					{
						"text-color-end",
						{"#ffffff",
						[&](std::string value) { gui->getGUIConfig()->getColor(value, textColorEnd); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(textColorEnd) }}); }}
					},
				}
			);
			textColor = targetTextColor;
		}
	}
	return true;
}

void gui::LabelWidget::revalidate()
{
	textColor.r += (targetTextColor.r - textColor.r) * transitionSpeed;
	textColor.g += (targetTextColor.g - textColor.g) * transitionSpeed;
	textColor.b += (targetTextColor.b - textColor.b) * transitionSpeed;

	if (revalidateSize)
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		wTarget = metrics.x + padding * (center + 1);
		hTarget = font->size + padding * (center + 1);
	}
	else
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		if (w == 0)
			wTarget = metrics.x + padding * (center + 1);
		if (h == 0)
			hTarget = font->size + padding * (center + 1);
	}
	Widget::revalidate();
}

gui::LabelWidget::LabelWidget(GUI* gui, nlohmann::json j)
	: Widget(gui), displayFont(gui->getFontManager()->defaultFont)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}