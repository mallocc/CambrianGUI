#include "DropdownListWidget.h"
#include "GUI.h"


bool gui::DropdownListWidget::init(nlohmann::json j, bool ignoreType)
{
	if (VLayoutWidget::init(j, true))
	{
		if (checkWidgetType<DropdownListWidget>(ignoreType))
		{
			m_visible = false;

			m_config["labelwidget"] = labelTemplate;
			m_config.load(j);
		}
	}

	return true;
}

bool gui::DropdownListWidget::initList(nlohmann::json j)
{
	clearChildren();

	std::cout << "gui::DropdownListWidget::initList() - j: " << j.dump(2) << std::endl;
	for (auto& d : j)
	{
		json_get_string(d, "text", text)
		{
			labelTemplate["text"] = text;
			labelTemplate["w"] = m_w;
			create_widget(widget, labelTemplate)
			{
				widget->onClick = [=](GUI* gui, MouseEventData mouseEventData)
				{
					nlohmann::json j;
					j["intent"] = "dropdown";
					j["data"] = d;
					this->onIntent(j);
				};
				addChild(widget);
			}
		}
	}

	return true;
}

void gui::DropdownListWidget::draw(float tx, float ty, bool editMode)
{
	if (!floating)
	{
		if (m_parent != nullptr)
		{
			m_parent->getAbsolutePosition(tx, ty);
			ty += m_parent->m_h;
		}
	}
	VLayoutWidget::draw(tx, ty, editMode);
}

gui::Widget* gui::DropdownListWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	if (!floating)
		if (m_parent != nullptr)
			mouseEventData.y -= m_parent->m_h;
	return VLayoutWidget::onMouseEvent(mouseEventData, process, focus);
}

void gui::DropdownListWidget::onIntent(nlohmann::json intent)
{
	if (m_parent != nullptr)
	{
		m_parent->onIntent(intent);
	}
	m_visible = false;
	floating = false;
}

gui::DropdownListWidget::DropdownListWidget(GUI* gui, nlohmann::json j) : VLayoutWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}
