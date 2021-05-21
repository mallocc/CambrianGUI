#pragma once

#include "Widget.h"

namespace gui
{
	enum ALIGNMENT
	{
		ALIGN_NONE,
		ALIGN_TOP,
		ALIGN_BOTTOM,
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_START,
		ALIGN_END,
		ALIGN_RSTART,
		ALIGN_REND,
		ALIGN_STACKED,
		ALIGN_SPACED,
		ALIGN_CENTERED,
		ALIGN_LIST,
		ALIGN_NUMBER,
	};

	enum SIZING
	{
		SIZE_INHERIT,
		SIZE_NONE,
		SIZE_EXPAND,
		SIZE_EXPAND_WIDTH,
		SIZE_EXPAND_HEIGHT,
		SIZE_NUMBER
	};

	const static std::string SIZE_STRINGS[static_cast<size_t>(SIZING::SIZE_NUMBER)] = {
		"inherit",
		"none",
		"expand",
		"expand-width",
		"expand-height",
	};

	const static std::string ALIGN_STRINGS[static_cast<size_t>(ALIGNMENT::ALIGN_NUMBER)] = {
		"none",
		"top",
		"bottom",
		"left",
		"right",
		"start",
		"end",
		"rstart",
		"rend",
		"stacked",
		"spaced",
		"centered",
		"list",
	};

	struct ContainerWidget : Widget, WidgetType<ContainerWidget>
	{
		DEFINE_WIDGET_TYPE("container");

		ALIGNMENT alignment = ALIGNMENT::ALIGN_NONE;
		SIZING sizing = SIZING::SIZE_INHERIT;
		float padding = 0.0f;
		float spacing = 0.0f;
		bool childEnvoke = false;
		std::vector<Widget*> children;
		std::vector<Widget*> radioChildren;
		Widget* lastLocalWidgetHandled = nullptr;

		void clearChildren();

		std::vector<Widget*> getVisibleChildren();

		void addChild(Widget* component);
		void addRadioChild(Widget* component);

		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);

		virtual bool onLeaveEvent(MouseEventData mouseEventData, bool process);
		virtual Widget* onKeyEvent(KeyEventData keyEventData);

		virtual void draw(float tx, float ty, bool editMode = false);

		virtual bool init(nlohmann::json j, bool ignoreType = false);

		virtual nlohmann::json toJson();

		virtual void revalidate();

		virtual void expand();

		virtual void onIntent(nlohmann::json intent) override;

		ContainerWidget(GUI* gui, nlohmann::json j);

		virtual int32_t getChildIndex(Widget* child);

		virtual std::vector<Widget*> getCheckedChildren();
	};
}