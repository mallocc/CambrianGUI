#include "DropdownListWidget.h"
#include "GUI.h"


bool gui::DropdownListWidget::init(nlohmann::json j, bool ignoreType)
{
	if (VLayoutWidget::init(j, true))
	{
		if (checkWidgetType<DropdownListWidget>(ignoreType))
		{
			hide();

			ConfigList fields;
			{
				fields["labelwidget"] = labelTemplate;
			}
			fields.load(j);
			m_config += fields;
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
			labelTemplate["w"] = W();
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
		if (getParent() != nullptr)
		{
			getParent()->getAbsolutePosition(tx, ty);
			ty += getParent()->H();
		}
	}
	VLayoutWidget::draw(tx, ty, editMode);
}

gui::Widget* gui::DropdownListWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	if (!floating)
		if (getParent() != nullptr)
			mouseEventData.y -= getParent()->H();
	return VLayoutWidget::onMouseEvent(mouseEventData, process, focus);
}

void gui::DropdownListWidget::onIntent(nlohmann::json intent)
{
	if (getParent() != nullptr)
	{
		getParent()->onIntent(intent);
	}
	hide();
	floating = false;
}

gui::DropdownListWidget::DropdownListWidget(GUI* gui, nlohmann::json j) : VLayoutWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}
