#include "DropdownWidget.h"
#include "GUI.h"

bool gui::DropdownWidget::init(nlohmann::json j, bool ignoreType)
{
	if (HLayoutWidget::init(j, true))
	{
		if (checkWidgetType<DropdownWidget>(ignoreType))
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

			create_widget_as(LabelWidget, labelWidget, labelJson)
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
			alignment = ALIGN_SPACED;
			sizing = SIZE_NONE;
		}
	}

	return true;
}

void gui::DropdownWidget::onIntent(nlohmann::json intentData)
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

void gui::DropdownWidget::openDropdown()
{
	if (icon != nullptr)
		icon->check();
	getGUI()->openDropdownIntent(this, this->data);
}

nlohmann::json gui::DropdownWidget::toJson()
{
	return Widget::toJson();
}

gui::DropdownWidget::DropdownWidget(GUI* gui, nlohmann::json j) : HLayoutWidget(gui, j)
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
