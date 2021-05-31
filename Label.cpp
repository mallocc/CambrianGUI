#include <GL/glew.h>
#include "Label.h"
#include "GUI.h"

void gui::Label::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);

	tx += X(); ty += Y();

	// Draw title
	glColor4f(textColor.r, textColor.g, textColor.b, textColor.a);

	auto font = displayFont;
	auto metrics = font->textMetrics(text, 1.0f);

	float newWidth = W();
	float newHeight = H();

	glPushMatrix();
	glTranslatef(std::round(tx), std::round(ty), 0);
	// centered
	if (center)
	{
		font->renderText(
			text,
			getOffsetX() + newWidth/2.0f -metrics.x / 2.0f,
			getOffsetY() + newHeight/2.0f -metrics.y / 2.0f, 1., bold, italic);
	}
	else
	{
		newWidth = metrics.x + padding * 2.0f;
		newHeight = metrics.y + padding * 2.0f;
		font->renderText(
			text,
			getOffsetX() + padding,
			getOffsetY() + padding, 1., bold, italic);
	}
	glPopMatrix();

	if (editMode)
	{
		glColor3f(0, 0, 1);

		glBegin(GL_LINES);
		{
			glVertex2f(tx, 0);
			glVertex2f(tx, getGUI()->h);

			glVertex2f(0, ty);
			glVertex2f(getGUI()->w, ty);
		}
		glEnd();
	}
	glColor4f(1, 1, 1, 1);
}

bool gui::Label::init(const nlohmann::json& j, bool ignoreType)
{
	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<Label>(ignoreType))
		{
			ConfigList fields;
			{
				fields["font"] = {
				"OpenSans@24",
				[&](std::string value) { displayFont = getGUI()->getFontManager()->fonts[value] == nullptr ? getGUI()->getFontManager()->defaultFont : getGUI()->getFontManager()->fonts[value]; },
				[&](std::string fieldName) { return nlohmann::json({{ fieldName, displayFont->fontName }}); }
				};
				fields["revalidate-size"] = revalidateSize;
				fields["center"] = center;
				fields["bold"] = bold;
				fields["italic"] = italic;
				fields["padding"] = padding;
				fields["text-color"] = colorConfigItem(targetTextColor);
				fields["text-color-start"] = colorConfigItem(textColorStart);
				fields["text-color-end"] = colorConfigItem(textColorEnd);
				fields["text"] = text;
			}

			fields.load(j);

			m_config += fields;

			textColor = targetTextColor;
		}
	}
	return true;
}

void gui::Label::revalidate()
{
	textColor.r += (targetTextColor.r - textColor.r) * getTransitionSpeed();
	textColor.g += (targetTextColor.g - textColor.g) * getTransitionSpeed();
	textColor.b += (targetTextColor.b - textColor.b) * getTransitionSpeed();

	Widget::revalidate();

	if (revalidateSize)
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		setW(metrics.x + padding * 2.0f);
		setH(metrics.y + padding * 2.0f);
	}
	else
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		if (W() == 0)
			setW(metrics.x + padding * 2.0f);
		if (H() == 0)
			setH(metrics.y + padding * 2.0f);
	}
}

gui::Label::Label(GUI* gui)
	: Widget(gui), displayFont(gui->getFontManager()->defaultFont)
{}