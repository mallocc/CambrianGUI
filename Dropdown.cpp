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
					nlohmann::json j;
					j["intent"] = "dropdown";
					j["data"] = d;
					onIntent(j);
					defaultChoiceFound = true;
					break;
				}
				count++;
			}
			if (!defaultChoiceFound)
			{
				for (auto& d : data)
				{
					nlohmann::json j;
					j["intent"] = "dropdown";
					j["data"] = d;
					onIntent(j);
					break;
				}
			}
			setAlignment(ALIGN_SPACED);
			setSizing(SIZE_NONE);
		}
	}

	return true;
}

void gui::Dropdown::onIntent(nlohmann::json intentData)
{
	if (icon != nullptr)
		icon->uncheck();

	json_get_string(intentData, "intent", intent)
	{
		if (intent == "dropdown")
		{
			json_get_object(intentData, "data", data)
			{			
				json_get_string(data, "text", text)
				{
					if (label != nullptr)
						label->text = text;
				}
			}
		}
	}
}

void gui::Dropdown::openDropdown()
{
	if (icon != nullptr)
		icon->check();
	getGUI()->openDropdownIntent(this, this->data);
}

nlohmann::json gui::Dropdown::toJson()
{
	return Widget::toJson();
}

gui::Dropdown::Dropdown(GUI* gui, nlohmann::json j) : HLayout(gui, j)
{
	onClick = [&](GUI* gui, MouseEventData mouseEventData)
	{
		this->openDropdown();
	};
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}
