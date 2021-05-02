#pragma once

#include "Configuration.h"
#include "Widget.h"

namespace gui
{
	typedef std::function<Widget* (GUI*, nlohmann::json)> StringToWidgetFunction;

	class WidgetManager
	{
	public:
		WidgetManager(GUI* gui, Configuration* config);
		bool init();
		Widget* findWidget(std::string id);
		Widget* createWidget(nlohmann::json& j);
		bool removeWidget(std::string id);
		void handleDynamicJson(nlohmann::json j, std::string id);
		void bringForwards(Widget* widget);
		void bringToFront(Widget* widget);
		void sendBackwards(Widget* widget);
		void sendToBack(Widget* widget);

		Widget* getRootWidget();
		Widget* getFloatingLabelWidget();
		Widget* getDropDownListWidget();

		template <typename T>
		void registerWidget(std::string classname);
	private:
		Configuration* config = nullptr;
		std::map<std::string, Widget*> widgetMap;
		GUI* gui = nullptr;

		Widget* rootWidget = nullptr;
		Widget* floatingLabelWidget = nullptr;
		Widget* dropDownListWidget = nullptr;

		std::map<std::string, StringToWidgetFunction> stringToWidgetFunctions;

		void createFloatingLabelWidget();

		void createDropDownListWidget();
	};

	template <typename T>
	void gui::WidgetManager::registerWidget(std::string classname)
	{
		StringToWidgetFunction allocate = [](GUI* a, nlohmann::json b) { return new T(a, b); };
		stringToWidgetFunctions[classname] = allocate;
		std::cout << "Registered new widget class: " << classname << std::endl;
	}

/**
	Create a new scope when creating a new widget.
	create_widget(WIDGET_SYMBOL, JSON_SYMBOL)
	>	Widget* WIDGET_SYMBOL = gui->getWidgetManager()->createWidget(JSON_SYMBOL);
	>	if (WIDGET_SYMBOL != nullptr)
*/
#define create_widget(WIDGET_SYMBOL, JSON_SYMBOL) \
		Widget* WIDGET_SYMBOL = gui->getWidgetManager()->createWidget(JSON_SYMBOL); \
		if (WIDGET_SYMBOL != nullptr)

/**
	Create a new scope when finding a widget.
	find_widget(WIDGET_SYMBOL, ID_SYMBOL)
	>	Widget* WIDGET_SYMBOL = gui->getWidgetManager()->findWidget(ID_SYMBOL);
	>	if (WIDGET_SYMBOL != nullptr)
*/
#define find_widget(WIDGET_SYMBOL, ID_SYMBOL) \
		Widget* WIDGET_SYMBOL = gui->getWidgetManager()->findWidget(ID_SYMBOL); \
		if (WIDGET_SYMBOL != nullptr)
/**
	Create a new scope when getting widget of specific type.
	widget_as(TYPE_SYMBOL, WIDGET_SYMBOL)
	>	TYPE_SYMBOL* WIDGET_SYMBOL = dynamic_cast<TYPE_SYMBOL*>(WIDGET_SYMBOL);
	>	if (WIDGET_SYMBOL != nullptr)
*/
#define widget_as(TYPE_SYMBOL, WIDGET_SYMBOL) \
		TYPE_SYMBOL* WIDGET_SYMBOL = dynamic_cast<TYPE_SYMBOL*>(WIDGET_SYMBOL); \
		if (WIDGET_SYMBOL != nullptr)
/**
	Create a new scope when finding a widget of specific type.
	find_widget_as(TYPE_SYMBOL, WIDGET_SYMBOL, ID_SYMBOL)
	>	TYPE_SYMBOL* WIDGET_SYMBOL = dynamic_cast<TYPE_SYMBOL*>(gui->getWidgetManager()->findWidget(ID_SYMBOL));
	>	if (WIDGET_SYMBOL != nullptr)
*/
#define find_widget_as(TYPE_SYMBOL, WIDGET_SYMBOL, ID_SYMBOL) \
		TYPE_SYMBOL* WIDGET_SYMBOL = dynamic_cast<TYPE_SYMBOL*>(gui->getWidgetManager()->findWidget(ID_SYMBOL)); \
		if (WIDGET_SYMBOL != nullptr)
}