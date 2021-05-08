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
			config["font"] = {
				"OpenSans@24",
				[&](std::string value) { displayFont = gui->getFontManager()->fonts[value] == nullptr ? gui->getFontManager()->defaultFont : gui->getFontManager()->fonts[value]; },
				[&](std::string fieldName) { return nlohmann::json({{ fieldName, displayFont->fontName }}); }
			};
			config["revalidate-size"]  = revalidateSize;
			config["center"]           = center;
			config["bold"]             = bold;
			config["italic"]           = italic;
			config["padding"]          = padding;
			config["text-color"]       = colorConfigItem(targetTextColor);
			config["text-color-start"] = colorConfigItem(textColorStart);
			config["text-color-end"]   = colorConfigItem(textColorEnd);

			config.load(j);

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