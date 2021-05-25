#pragma once

#include "Widget.h"

namespace gui
{
	class Container : public Widget, public WidgetType<Container>
	{
		DEFINE_WIDGET_TYPE("container");
	public:
		Container(GUI* gui);
		
		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);

		virtual Widget* onKeyEvent(KeyEventData keyEventData);

		virtual bool onLeaveEvent(MouseEventData mouseEventData, bool process);

		virtual void draw(float tx, float ty, bool editMode = false);

		virtual bool init(nlohmann::json j, bool ignoreType = false);

		virtual nlohmann::json toJson();

		virtual void revalidate();

		virtual void onIntent(nlohmann::json intent) override;

		void clearChildren();

		void addChild(Widget* component);

		void addRadioChild(Widget* component);

		void setChildEnvoke(bool childEnvoke = true);
		bool isChildEnvoke();

		void setForAllWidgets(std::function<void(Widget*)> set);
		Widget* searchForAllWidgets(std::function<Widget*(Widget*)> search);
		void getForAllWidgets(std::function <nlohmann::json(Widget*, nlohmann::json& data)> get, nlohmann::json& data);

		virtual std::vector<Widget*>& getCheckedChildren();
		std::vector<Widget*>& getVisibleChildren();
		virtual int32_t getChildIndex(Widget* child);
		std::vector<Widget*>& getRadioChildren();
		std::vector<Widget*>& getChildren();
	private:
		bool m_childEnvoke = false;
		std::vector<Widget*> m_children;
		std::vector<Widget*> m_radioChildren;
		Widget* m_lastLocalWidgetHandled = nullptr;
	};
}