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

		void registerWidget(std::string classname, StringToWidgetFunction function);
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
}