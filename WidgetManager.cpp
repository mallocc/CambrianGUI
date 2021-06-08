#include "WidgetManager.h"
#include "Configuration.h"
#include "Layout.h"
#include "Label.h"
#include "DropdownList.h"

#include <sstream>
#include <regex>

namespace
{
	std::string replaceString(std::string subject, const std::string& search,
		const std::string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}
}


gui::WidgetManager::WidgetManager(GUI* gui, Configuration* config)
	: gui(gui),
	widgetMap(),
	config(config),
	rootWidget(nullptr)
{
}

bool gui::WidgetManager::init()
{
	bool success = true;

	nlohmann::json& j = config->getConfig();
	widgetMap.clear();

	if (success &= checkJSON(j, "root-widget"))
	{
		if (rootWidget != nullptr)
		{
			delete rootWidget;
		}

		rootWidget = createWidget(j.at("root-widget"));
	}
	else
	{
		std::cout << "There were errors in reading config for GUI" << std::endl;
	}

	createFloatingLabelWidget();
	createDropDownListWidget();

	return success;
}

gui::Widget* gui::WidgetManager::findWidget(std::string id)
{
	std::map<std::string, Widget*>::iterator itor = widgetMap.find(id);
	if (itor != widgetMap.end())
		return itor->second;
	return nullptr;
}

gui::Widget* gui::WidgetManager::createWidget(nlohmann::json j)
{
	Widget* widget = nullptr;
	std::string tmp;

	if (readJSON(j, "class", tmp))
	{
		/*
		Look for defined variables with the @...@ syntax,
		If we find one, add it to our map
		*/

		localVariables_t localVariables;
		for (auto search : j.items())
		{
			if (Configuration::isLocalVariable(search.key()))
			{
				std::string id;
				if (readJSON(j, "id", id))
					localVariables[id + "-" + search.key()] = search.value();
			}
			else if (Configuration::isVariable(search.key()))
			{
				localVariables[search.key()] = search.value();
			}
		}

		//std::cout << "Substituting for " << tmp << std::endl;
		nlohmann::json i;
		if (config != nullptr)
		{
			if (config->getClass(tmp, i))
			{
				globProperties(i, j);

				for (auto& lv : localVariables)
					replaceVariables(i, lv.first, lv.second);

				widget = createWidget(i);
			}
		}
	}
	else if (readJSON(j, "widget", tmp))
	{
		if (stringToWidgetFunctions.find(tmp) != stringToWidgetFunctions.end())
		{
			widget = stringToWidgetFunctions.at(tmp)(gui);
			if (widget != nullptr)
			{
				if (!widget->init(j))
				{
					std::cout << "Failed to init widget: " << widget->getWidgetType() << std::endl;
				}
			}
			//std::cout << "Found registered widget:" << tmp << std::endl;
		}
		else
		{
			std::cout << "Failed to find registered widget:" << tmp << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to find widget field" << std::endl;
	}

	if (widget != nullptr)
	{
		// Reset widget id to default if one already exists
		if (findWidget(widget->getId()) != nullptr)
		{
			widget->setId("");
		}

		// If default id, generate a unique id
		if (widget->getId() == "")
		{
			uint32_t id = 0U;
			bool found = false;
			while (!found)
			{
				std::stringstream newId;
				newId << widget->getType() << id;
				if (findWidget(newId.str()) == nullptr)
				{
					widget->setId(newId.str());
					found = true;
				}
				id++;
			}
		}

		// Add to map
		widgetMap[widget->getId()] = widget;
	}

	return widget;
}

bool gui::WidgetManager::removeWidget(std::string id)
{
	bool removed = false;
	std::map<std::string, Widget*>::iterator itor = widgetMap.find(id);
	if (itor != widgetMap.end())
	{
		delete itor->second;
		widgetMap.erase(itor);
		removed = true;
	}
	return removed;
}

void gui::WidgetManager::handleDynamicJson(nlohmann::json j, std::string id)
{
	for (auto& i : j)
	{
		std::string tmp;
		if (readJSONAsString(i, "target-id", tmp))
		{
			if (id != tmp)
			{
				Widget* widget = findWidget(tmp);
				if (widget)
				{
					std::cout << "found target widget: " << tmp << std::endl;
					readJSONAsString(i, "RESET", tmp);
					bool reset = tmp == "true";
					widget->getConfig().load(reset ? widget->getDefaultJson() : i, !reset);

					readJSONAsString(i, "CHECK", tmp);
					bool checked = tmp == "true";
					bool unchecked = tmp == "false";
					if (checked)
					{
						widget->check(true);
					}
					if (unchecked)
					{
						widget->uncheck(true);
					}
				}
				else
				{
					std::cout << "didnt find target widget: " << tmp << std::endl;
				}
			}
		}
	}
}

void gui::WidgetManager::bringForwards(Widget* widget)
{
	if (widget != nullptr)
	{
		if (widget->getParent() != nullptr)
		{
			Layout* layout = dynamic_cast<Layout*>(widget->getParent());
			if (layout != nullptr)
			{
				size_t position = 0U;
				auto& children = layout->getChildren();
				for (size_t i = 0U; i < children.size(); ++i)
					if (children[i] == widget)
						position = i;
				if (position < children.size() - 1)
				{
					std::vector<Widget*>::iterator itor = children.begin() + position;
					children.erase(itor);
					itor = children.begin() + position + 1;
					children.insert(itor, widget);
				}
			}
		}
	}
}

void gui::WidgetManager::bringToFront(Widget* widget)
{
	if (widget != nullptr)
	{
		if (widget->getParent() != nullptr)
		{
			Layout* layout = dynamic_cast<Layout*>(widget->getParent());
			if (layout != nullptr)
			{
				size_t position = 0UL;
				auto& children = layout->getChildren();
				for (size_t i = 0UL; i < children.size(); ++i)
					if (children[i] == widget)
						position = i;
				if (position > 0UL)
				{
					std::vector<Widget*>::iterator itor = children.begin() + position;
					children.erase(itor);
					itor = children.begin();
					children.insert(itor, widget);
				}
			}
		}
	}
}

void gui::WidgetManager::sendBackwards(Widget* widget)
{
	if (widget != nullptr)
	{
		if (widget->getParent() != nullptr)
		{
			Layout* layout = dynamic_cast<Layout*>(widget->getParent());
			if (layout != nullptr)
			{
				size_t position = 0UL;
				auto& children = layout->getChildren();
				for (size_t i = 0UL; i < children.size(); ++i)
					if (children[i] == widget)
						position = i;
				if (position > 0UL)
				{
					std::vector<Widget*>::iterator itor = children.begin() + position;
					children.erase(itor);
					itor = children.begin() + position - 1;
					children.insert(itor, widget);
				}
			}
		}
	}
}

void gui::WidgetManager::sendToBack(Widget* widget)
{
	if (widget != nullptr)
	{
		if (widget->getParent() != nullptr)
		{
			Layout* layout = dynamic_cast<Layout*>(widget->getParent());
			if (layout != nullptr)
			{
				size_t position = 0U;
				auto& children = layout->getChildren();
				for (size_t i = 0U; i < children.size(); ++i)
					if (children[i] == widget)
						position = i;
				if (position < children.size() - 1)
				{
					std::vector<Widget*>::iterator itor = children.begin() + position;
					children.erase(itor);
					itor = children.end();
					children.insert(itor, widget);
				}
			}
		}
	}
}

gui::Widget* gui::WidgetManager::getRootWidget()
{
	return rootWidget;
}

gui::Widget* gui::WidgetManager::getFloatingLabelWidget()
{
	return floatingLabelWidget;
}

gui::Widget* gui::WidgetManager::getDropDownListWidget()
{
	return dropDownListWidget;
}

void gui::WidgetManager::createFloatingLabelWidget()
{
	nlohmann::json& j = config->getConfig();
	if (checkJSON(j, "floating-label"))
	{
		floatingLabelWidget = dynamic_cast<Label*>(createWidget(j.at("floating-label")));
		if (floatingLabelWidget != nullptr)
		{
			floatingLabelWidget->revalidate();
			floatingLabelWidget->hide();
			Layout* widget = dynamic_cast<Layout*>(findWidget("mainlayout"));
			if (widget)
				widget->addChild(floatingLabelWidget);
			bringToFront(floatingLabelWidget);
		}
	}
	else
	{
		std::cout << "Could not location floating-label in config" << std::endl;
	}
}

void gui::WidgetManager::createDropDownListWidget()
{
	nlohmann::json& j = config->getConfig();
	if (checkJSON(j, "dropdown-list"))
	{
		dropDownListWidget = dynamic_cast<DropdownList*>(createWidget(j.at("dropdown-list")));
		if (dropDownListWidget != nullptr)
		{
			dropDownListWidget->revalidate();
			dropDownListWidget->hide();
			Layout* widget = dynamic_cast<Layout*>(findWidget("mainlayout"));
			if (widget)
			{
				widget->addChild(dropDownListWidget);
				bringToFront(dropDownListWidget);
			}
		}
	}
	else
	{
		std::cout << "Could not location dropdown-list in config" << std::endl;
	}
}