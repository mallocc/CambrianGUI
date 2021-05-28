#include "Dropdown.h"
#include "GUI.h"

bool gui::Dropdown::init(nlohmann::json j, bool ignoreType)
{
	if (HLayout::init(j, true))
	{
		if (checkWidgetType<Dropdown>(ignoreType))
		{
			ConfigList fields;
			{
				fields["data"] = data;
				fields["iconwidget"] = iconJson;
				fields["labelwidget"] = labelJson;
				fields["default-choice"] = defaultChoice;
			}
			fields.load(j);
			m_config += fields;

			create_widget_as(Label, labelWidget, labelJson)
			{
				addChild(labelWidget);
				label = labelWidget;
			}

			create_widget(iconWidget, iconJson)
			{
				addChild(iconWidget);
				icon = iconWidget;
			}

			uint32_t count = 0U;
			bool defaultChoiceFound = false;
			for (auto& d : data)
			{
				if (defaultChoice == count)
				{
					onSelectionEvent(d);
					defaultChoiceFound = true;
					break;
				}
				count++;
			}
			if (!defaultChoiceFound)
			{
				for (auto& d : data)
				{
					onSelectionEvent(d);
					break;
				}
			}
		}
	}

	return true;
}

bool gui::Dropdown::onClickEvent(MouseEventData mouseEventData, bool process)
{
	bool handled = HLayout::onClickEvent(mouseEventData, process);
	if (handled && process)
	{
		this->openDropdown();
	}
	return handled;
}

void gui::Dropdown::onSelectionEvent(nlohmann::json data)
{
	if (icon != nullptr)
		icon->uncheck();

	json_get_string(data, "text", text)
	{
		if (label != nullptr)
			label->text = text;
	}

	onSelection(data);
}

void gui::Dropdown::openDropdown()
{
	if (icon != nullptr)
		icon->check();
	getGUI()->openDropdownIntent(this, this->data, [&](nlohmann::json data)
		{
			this->onSelectionEvent(data);
		});
}

void gui::Dropdown::closeDropdown()
{
	if (icon != nullptr)
		icon->uncheck();
}

nlohmann::json gui::Dropdown::toJson()
{
	return Widget::toJson();
}

gui::Dropdown::Dropdown(GUI* gui) : HLayout(gui) 
{
	onSelection = [](nlohmann::json) {};
}


