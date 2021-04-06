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

	const static std::string SIZE_STRINGS[SIZE_NUMBER] = {
		"inherit",
		"none",
		"expand",
		"expand-width",
		"expand-height",
	};

	const static std::string ALIGN_STRINGS[ALIGN_NUMBER] = {
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

	const std::string STR_LAYOUTWIDGET = "layout";

	struct LayoutWidget : Widget
	{
		ALIGNMENT alignment = ALIGN_NONE;
		SIZING sizing = SIZE_INHERIT;
		float padding = 0.0f;
		float spacing = 0.0f;
		bool radio = false;		
		bool childEnvoke = false;
		std::vector<Widget*> children;
		Widget* lastLocalWidgetHandled = nullptr;

		void clearChildren();

		std::vector<Widget*> getVisibleChildren();

		void addChild(Widget* component);

		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);

		virtual bool onLeaveEvent(MouseEventData mouseEventData, bool process);
		virtual Widget* onKeyEvent(KeyEventData keyEventData);

		virtual void draw(float tx, float ty, bool editMode = false);

		virtual bool init(nlohmann::json j, bool ignoreType = false);

		virtual nlohmann::json toJson();

		virtual void revalidate();

		virtual void expand();

		virtual void onIntent(std::string intentChoice) override;

		LayoutWidget(GUI* gui, nlohmann::json j);

		virtual int32_t getChildIndex(Widget* child);
	};
}