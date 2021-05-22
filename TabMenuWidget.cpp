#include "TabMenuWidget.h"
#include "GUI.h"
#include "Utilities.h"
#include "LabelWidget.h"

bool gui::TabMenuWidget::init(nlohmann::json j, bool ignoreType)
{
	if (ContainerWidget::init(j, true))
	{
		if (checkWidgetType<TabMenuWidget>(ignoreType))
		{
			m_config["tabwidget"] = tabLabelTemplate;
			m_config.load(j);

			nlohmann::json mainLayoutJ;
			mainLayoutJ["widget"] = "vlayout";
			mainLayoutJ["align"] = "start";
			ContainerWidget* mainLayout = dynamic_cast<ContainerWidget*>(gui->getWidgetManager()->createWidget(mainLayoutJ));

			nlohmann::json tabMenuJ;
			tabMenuJ["widget"] = "hlayout";
			tabMenuJ["align"] = "start";
			tabMenuJ["size"] = "expand-height";
			tabMenu = dynamic_cast<ContainerWidget*>(gui->getWidgetManager()->createWidget(tabMenuJ));

			if (mainLayout != nullptr && tabMenu != nullptr)
			{
				addChild(mainLayout);
				mainLayout->addChild(tabMenu);

				nlohmann::json tabPaneJ;
				tabPaneJ["widget"] = "hlayout";
				tabPane = dynamic_cast<ContainerWidget*>(gui->getWidgetManager()->createWidget(tabPaneJ));

				if (tabPane != nullptr)
				{
					//tabPane->backgroundTex = tabSelectionTex;
					mainLayout->addChild(tabPane);

					if (checkJSON(j, "tabs"))
					{
						nlohmann::json jc = j.at("tabs");
						for (auto& i : jc)
						{
							create_widget_as(LabelWidget, child, i)
							{
								child->m_visible = false;
								tabPane->addChild(child);

								tabLabelTemplate["text"] = child->text;

								create_widget_as(LabelWidget, label, tabLabelTemplate)
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

					if (tabPane->children.size() > 0)
					{
						tabPane->children[0]->m_visible = true;
					}
					if (tabMenu->children.size() > 0)
					{
						tabMenu->children[0]->check();
					}
				}
			}
		}
	}

	return true;
}

void gui::TabMenuWidget::onIntent(nlohmann::json intent)
{
	std::string tab;
	readJSON(intent, "tab", tab);
	if (tabPane != nullptr)
	{
		for (auto& widget : tabPane->children)
			if (widget != nullptr)
			{
				widget_as(LabelWidget, labelWidget, widget)
				{
					if (labelWidget->text == tab)
						labelWidget->m_visible = true;
					else
						labelWidget->m_visible = false;
				}
			}
	}
	if (tabMenu != nullptr)
	{
		for (Widget* widget : tabMenu->children)
			if (widget != nullptr)
			{
				widget_as(LabelWidget, labelWidget, widget)
				{
					if (labelWidget->text == tab)
						labelWidget->check();
					else
						labelWidget->uncheck();
				}
			}
	}
}

gui::TabMenuWidget::TabMenuWidget(GUI* gui, nlohmann::json j) : ContainerWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}