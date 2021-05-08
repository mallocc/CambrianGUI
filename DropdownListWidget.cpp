#include "DropdownListWidget.h"
#include "GUI.h"


bool gui::DropdownListWidget::init(nlohmann::json j, bool ignoreType)
{
	if (VLayoutWidget::init(j, true))
	{
		if (checkWidgetType<DropdownListWidget>(ignoreType))
		{
			visible = false;

			config["labelwidget"] = labelTemplate;
			config.load(j);
		}
	}

	return true;
}

bool gui::DropdownListWidget::initList(nlohmann::json j)
{
	clearChildren();

	for (auto& d : j)
	{
		json_get_string(d, "text", text)
		{
			labelTemplate["text"] = text;
			labelTemplate["w"] = w;
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
	if (parent != nullptr)
	{
		parent->getAbsolutePosition(tx, ty);
		ty += parent->h;
	}
	VLayoutWidget::draw(tx, ty, editMode);
}

gui::Widget* gui::DropdownListWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	if (parent != nullptr)
		mouseEventData.y -= parent->h;
	return VLayoutWidget::onMouseEvent(mouseEventData, process, focus);
}

void gui::DropdownListWidget::onIntent(nlohmann::json intent)
{
	if (parent != nullptr)
	{
		parent->onIntent(intent);
	}
	visible = false;
}

gui::DropdownListWidget::DropdownListWidget(GUI* gui, nlohmann::json j) : VLayoutWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}
