#include "FormLayout.h"
#include "WidgetManager.h"
#include "HLayout.h"
#include "GUI.h"

gui::FormLayout::FormLayout(GUI* gui) : VLayout(gui)
{
}

bool gui::FormLayout::initList(nlohmann::json j)
{
	clearChildren();

	for (auto& row : j)
	{
		nlohmann::json rowLayoutJson;
		rowLayoutJson["widget"] = "hlayout";
		rowLayoutJson["align"].push_back("stacked");
		rowLayoutJson["align"].push_back("center");
		rowLayoutJson["expand"] = "prefered";
		create_widget_as(HLayout, rowLayout, rowLayoutJson)
		{
			json_get_string(row, "text", text)
			{
				nlohmann::json labelJson = labelTemplate;
				labelJson["text"] = text;
				create_widget(label, labelJson)
				{
					rowLayout->addChild(label);
				}

				json_get_object(row, "input", inputWidgetJson)
				{
					create_widget(inputWidget, inputWidgetJson)
					{
						rowLayout->addChild(inputWidget);
					}
				}
			}
			addChild(rowLayout);
		}
	}
	return true;
}

bool gui::FormLayout::init(nlohmann::json j, bool ignoreType)
{
	if (VLayout::init(j, true))
	{
		if (checkWidgetType<FormLayout>(ignoreType))
		{
			ConfigList fields;
			{
				fields["items"] = data;
				fields["label"] = labelTemplate;
				fields.load(j);
			}
			m_config += fields;

			initList(data);
		}
	}
	return true;
}
