#include "TabMenuWidget.h"
#include "GUI.h"
#include "Utilities.h"
#include "LabelWidget.h"

bool gui::TabMenuWidget::init(nlohmann::json j, bool ignoreType)
{
	if (ContainerWidget::init(j, true))
	{
		if (doesTypeMatch(ignoreType))
		{
			addToManifestList(j,
				{
					{
						"tabwidget",
						{"",
						[&](std::string value) { tabLabelTemplate = nlohmann::json::parse(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, tabLabelTemplate.dump(4) }}); }}
					},
				}
			);

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
							Widget* child = gui->getWidgetManager()->createWidget(i);
							child->visible = false;
							tabPane->addChild(child);

							tabLabelTemplate["text"] = child->text;
							LabelWidget* label = dynamic_cast<LabelWidget*>(gui->getWidgetManager()->createWidget(tabLabelTemplate));
							if (label != nullptr)
							{
								label->onClick = [=](GUI* gui, MouseEventData mouseEventData)
								{
									nlohmann::json j;
									j["tab"] = label->text;
									this->onIntent(j);
								};
							}
							tabMenu->addChild(label);
						}
					}

					if (tabPane->children.size() > 0)
					{
						tabPane->children[0]->visible = true;
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
		for (Widget* widget : tabPane->children)
			if (widget != nullptr)
			{
				if (widget->text == tab)
					widget->visible = true;
				else
					widget->visible = false;
			}
	}
	if (tabMenu != nullptr)
	{
		for (Widget* widget : tabMenu->children)
			if (widget != nullptr)
			{
				if (widget->text == tab)
					widget->check();
				else
					widget->uncheck();
			}
	}
}

gui::TabMenuWidget::TabMenuWidget(GUI* gui, nlohmann::json j) : ContainerWidget(gui, j)
{
	setClassname(TABMENUWIDGET_CLASSNAME);
	init(j);
}