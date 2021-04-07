#include "DropdownWidget.h"
#include "GUI.h"

bool gui::DropdownWidget::init(nlohmann::json j, bool ignoreType)
{
	if (HLayoutWidget::init(j, true))
	{
		if (doesTypeMatch(ignoreType))
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
					onIntent(d.dump(4));
					defaultChoiceFound = true;
					break;
				}
				count++;
			}
			if (!defaultChoiceFound)
			{
				for (auto& d : data)
				{
					onIntent(d.dump(4));
					break;
				}
			}
		}
	}

	return true;
}

void gui::DropdownWidget::onIntent(std::string intentChoice)
{
	if (icon != nullptr)
		icon->toggleOff();
}

void gui::DropdownWidget::openDropdown()
{
	if (icon != nullptr)
		icon->toggleOn();
	gui->openDropdownIntent(this, this->data);
}

nlohmann::json gui::DropdownWidget::toJson()
{
	return Widget::toJson();
}

gui::DropdownWidget::DropdownWidget(GUI* gui, nlohmann::json j) : HLayoutWidget(gui, j)
{
	setClassname(DROPDOWNWIDGET_CLASSNAME);
	onClick = [&](GUI* gui, MouseEventData mouseEventData)
	{
		this->openDropdown();
	};
	init(j);	
}
