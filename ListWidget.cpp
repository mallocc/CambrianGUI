#include "ListWidget.h"
#include <GUI.h>

gui::List::List(GUI* gui, nlohmann::json j) : VLayout(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

bool gui::List::init(nlohmann::json j, bool ignoreType)
{
	bool success = false;

	if (VLayout::init(j, true))
	{
		if (checkWidgetType<List>(ignoreType))
		{
			ConfigList fields;
			{
				fields["data"] = data;
				fields.load(j);
			}

			m_config += fields;

			success = true;

			//nlohmann::json data = nlohmann::json::parse(dataStr);

			if (!data.empty() && data.is_array())
			{
				for (auto& itemData : data)
				{
					nlohmann::json tempJ;

					if (itemData.is_object())
					{
						tempJ = itemData;
					}
					else if (itemData.is_primitive() && !itemData.is_null())
					{
						tempJ["class"] = "testlabel";
						tempJ["text"] = itemData;
					}

					if (!tempJ.empty())
					{
						create_widget(widget, tempJ)
						{
							widget->onClick = [=](GUI* gui, MouseEventData mouseEventData) {
								nlohmann::json intent;
								intent["intent"] = "childClicked";
								intent["id"] = widget->getId();
								this->onIntent(intent);
							};
							addChild(widget);
							if (widget->isRadio())
								addRadioChild(widget);
						}
					}
				}
			}


			alignment = ALIGN_LIST;
		}
	}
	return success;
}

void gui::List::onIntent(nlohmann::json intentData)
{
	json_get_string(intentData, "intent", intent)
	{
		json_get_string(intentData, "id", childId)
		{
			if (intent == "radio" && this->isRadio())
			{
				for (Widget* widget : getVisibleChildren())
				{
					if (widget != nullptr)
						if (widget->isRadio())
							widget->uncheck(false);
				}

				for (Widget* widget : getVisibleChildren())
				{
					if (widget != nullptr)
						if (widget->isRadio())
							if (widget->getId() == childId)
								widget->check(false);
				}
			}
		}
	}
}
