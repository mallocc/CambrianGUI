#include "TabMenu.h"
#include "GUI.h"
#include "Utilities.h"
#include "Label.h"

bool gui::TabMenu::init(nlohmann::json j, bool ignoreType)
{
	if (Container::init(j, true))
	{
		if (checkWidgetType<TabMenu>(ignoreType))
		{
			ConfigList fields;
			{
				fields["tabwidget"] = tabLabelTemplate;
				fields.load(j);
			}
			m_config += fields;

			nlohmann::json mainLayoutJ;
			mainLayoutJ["widget"] = "vlayout";
			mainLayoutJ["align"] = "start";
			Container* mainLayout = dynamic_cast<Container*>(getGUI()->getWidgetManager()->createWidget(mainLayoutJ));

			nlohmann::json tabMenuJ;
			tabMenuJ["widget"] = "hlayout";
			tabMenuJ["align"] = "start";
			tabMenuJ["size"] = "expand-height";
			tabMenu = dynamic_cast<Container*>(getGUI()->getWidgetManager()->createWidget(tabMenuJ));

			if (mainLayout != nullptr && tabMenu != nullptr)
			{
				addChild(mainLayout);
				mainLayout->addChild(tabMenu);

				nlohmann::json tabPaneJ;
				tabPaneJ["widget"] = "hlayout";
				tabPane = dynamic_cast<Container*>(getGUI()->getWidgetManager()->createWidget(tabPaneJ));

				if (tabPane != nullptr)
				{
					//tabPane->backgroundTex = tabSelectionTex;
					mainLayout->addChild(tabPane);

					if (checkJSON(j, "tabs"))
					{
						nlohmann::json jc = j.at("tabs");
						for (auto& i : jc)
						{
							create_widget_as(Label, child, i)
							{
								child->hide();
								tabPane->addChild(child);

								tabLabelTemplate["text"] = child->text;

								create_widget_as(Label, label, tabLabelTemplate)
								{
									label->onClick = [=](GUI* gui, MouseEventData mouseEventData)
									{
										nlohmann::json j;
										j["tab"] = label->text;
										this->onIntent(j);
									};
									tabMenu->addChild(label);
								}
							}
						}
					}

					if (tabPane->getChildren().size() > 0)
					{
						tabPane->getChildren()[0]->show();
					}
					if (tabMenu->getChildren().size() > 0)
					{
						tabMenu->getChildren()[0]->check();
					}
				}
			}
		}
	}

	return true;
}

void gui::TabMenu::onIntent(nlohmann::json intent)
{
	std::string tab;
	readJSON(intent, "tab", tab);
	if (tabPane != nullptr)
	{
		for (auto& widget : tabPane->getChildren())
			if (widget != nullptr)
			{
				widget_as(Label, labelWidget, widget)
				{
					labelWidget->setVisible(labelWidget->text == tab);
				}
			}
	}
	if (tabMenu != nullptr)
	{
		for (Widget* widget : tabMenu->getChildren())
			if (widget != nullptr)
			{
				widget_as(Label, labelWidget, widget)
				{
					if (labelWidget->text == tab)
						labelWidget->check();
					else
						labelWidget->uncheck();
				}
			}
	}
}

gui::TabMenu::TabMenu(GUI* gui, nlohmann::json j) : Container(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}