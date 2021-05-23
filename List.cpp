#include "List.h"
#include <GUI.h>

gui::List::List(GUI* gui) : VLayout(gui) {}

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
								this->onItemClickedEvent(widget);
							};
							addChild(widget);
							if (widget->isRadio())
								addRadioChild(widget);
						}
					}
				}
			}
			setAlignment(ALIGN_LIST);
		}
	}
	return success;
}

void gui::List::onItemClickedEvent(Widget* clickedItem)
{
	if (this->isRadio())
	{
		for (Widget* widget : getVisibleChildren())
			if (widget != nullptr)
				if (widget->isRadio())
					widget->uncheck(false);

		if (clickedItem != nullptr)
			if (clickedItem->isRadio())
				clickedItem->check(false);
	}

	onItemClicked(clickedItem);
}
