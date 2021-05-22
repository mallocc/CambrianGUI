#include "ListWidget.h"
#include <GUI.h>

gui::ListWidget::ListWidget(GUI* gui, nlohmann::json j) : VLayoutWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

bool gui::ListWidget::init(nlohmann::json j, bool ignoreType)
{
	bool success = false;

	alignment = ALIGN_LIST;

	if (VLayoutWidget::init(j, true))
	{
		if (checkWidgetType<ListWidget>(ignoreType))
		{
			config["data"] = data;
			config.load(j);

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
								intent["id"] = widget->id;
								this->onIntent(intent);
							};
							addChild(widget);
							if (widget->radio)
								addRadioChild(widget);
						}
					}
				}
			}
		}
	}
	return success;
}

void gui::ListWidget::onIntent(nlohmann::json intentData)
{
	json_get_string(intentData, "intent", intent)
	{
		json_get_string(intentData, "id", childId)
		{
			if (intent == "radio" && this->radio)
			{
				for (Widget* widget : getVisibleChildren())
				{
					if (widget != nullptr)
						if (widget->radio)
							widget->uncheck(false);
				}

				for (Widget* widget : getVisibleChildren())
				{
					if (widget != nullptr)
						if (widget->radio)
							if (widget->id == childId)
								widget->check(false);
				}
			}
		}
	}
}
