#include "DropdownList.h"
#include "GUI.h"


bool gui::DropdownList::init(nlohmann::json j, bool ignoreType)
{
	if (VLayout::init(j, true))
	{
		if (checkWidgetType<DropdownList>(ignoreType))
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

bool gui::DropdownList::initList(nlohmann::json j)
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

void gui::DropdownList::draw(float tx, float ty, bool editMode)
{
	if (!floating)
	{
		if (getParent() != nullptr)
		{
			getParent()->getAbsolutePosition(tx, ty);
			ty += getParent()->H();
		}
	}
	VLayout::draw(tx, ty, editMode);
}

gui::Widget* gui::DropdownList::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	if (!floating)
		if (getParent() != nullptr)
			mouseEventData.y -= getParent()->H();
	return VLayout::onMouseEvent(mouseEventData, process, focus);
}

void gui::DropdownList::onIntent(nlohmann::json intent)
{
	if (getParent() != nullptr)
	{
		getParent()->onIntent(intent);
	}
	hide();
	floating = false;
}

gui::DropdownList::DropdownList(GUI* gui, nlohmann::json j) : VLayout(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}
