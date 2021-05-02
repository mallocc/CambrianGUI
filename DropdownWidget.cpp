#include "DropdownWidget.h"
#include "GUI.h"

bool gui::DropdownWidget::init(nlohmann::json j, bool ignoreType)
{
	if (HLayoutWidget::init(j, true))
	{
		if (checkWidgetType<DropdownWidget>(ignoreType))
		{
			alignment = ALIGN_SPACED;
			sizing = SIZE_NONE;
			addToManifestList(j,
				{
					{
						"data",
						{"",
						[&](std::string value) { data = nlohmann::json::parse(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, data.dump(4) }}); }}
					},
					{
						"iconwidget",
						{"",
						[&](std::string value) { iconJson = nlohmann::json::parse(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, iconJson.dump(4) }}); }}
					},
					{
						"labelwidget",
						{"",
						[&](std::string value) { labelJson = nlohmann::json::parse(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, labelJson.dump(4) }}); }}
					},
					{
						"default-choice",
						{"0",
						[&](std::string value) { defaultChoice = std::atoi(value.c_str()); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, defaultChoice }}); }}
					}
				}
			);

			icon = gui->getWidgetManager()->createWidget(iconJson);
			label = gui->getWidgetManager()->createWidget(labelJson);

			addChild(label);
			addChild(icon);

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
	gui->openDropdownIntent(this, this->data);
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
		std::cout << "Failed to init widget: " << getClassname() << std::endl;
	}
}
