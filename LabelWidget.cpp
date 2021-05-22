#include <GL/glew.h>
#include "LabelWidget.h"
#include "GUI.h"

void gui::LabelWidget::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, editMode);

	tx += m_x; ty += m_y;

	// Draw title
	glColor4f(textColor.r, textColor.g, textColor.b, m_opacity);

	auto font = displayFont;

	glPushMatrix();
	glTranslatef(std::round(tx), std::round(ty), 0);
	// centered
	if (center)
	{
		font->renderText(
			text,
			m_xOffset + m_w / 2.0f,
			m_yOffset + m_h / 2.0f - font->size / 2.0f + (font->size - 2.0f), 1., bold, italic, 1);
	}
	else
	{
		font->renderText(
			text,
			m_xOffset + padding,
			m_yOffset + m_h / 2.0f - font->size / 2.0f + (font->size - 2.0f), 1., bold, italic, 0);
	}
	glPopMatrix();

	if (editMode)
	{
		glColor3f(0, 0, 1);

		glBegin(GL_LINES);
		{
			glVertex2f(tx, 0);
			glVertex2f(tx, m_gui->h);

			glVertex2f(0, ty);
			glVertex2f(m_gui->w, ty);
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
			ConfigList fields;
			{
				fields["font"] = {
				"OpenSans@24",
				[&](std::string value) { displayFont = m_gui->getFontManager()->fonts[value] == nullptr ? m_gui->getFontManager()->defaultFont : m_gui->getFontManager()->fonts[value]; },
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

void gui::LabelWidget::revalidate()
{
	textColor.r += (targetTextColor.r - textColor.r) * m_transitionSpeed;
	textColor.g += (targetTextColor.g - textColor.g) * m_transitionSpeed;
	textColor.b += (targetTextColor.b - textColor.b) * m_transitionSpeed;

	if (revalidateSize)
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		m_wTarget = metrics.x + padding * (center + 1);
		m_hTarget = font->size + padding * (center + 1);
	}
	else
	{
		auto font = displayFont;
		auto metrics = font->textMetrics(text, 1.0f);
		if (m_w == 0)
			m_wTarget = metrics.x + padding * (center + 1);
		if (m_h == 0)
			m_hTarget = font->size + padding * (center + 1);
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