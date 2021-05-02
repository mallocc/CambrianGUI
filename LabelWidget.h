#pragma once

#include "Widget.h"
#include "FontManager.h"

namespace gui
{
	struct LabelWidget : Widget, WidgetBase<LabelWidget>
	{
		static std::string getClassname()
		{
			return "label";
		}
		Font* displayFont = nullptr;
		bool center = false;
		bool revalidateSize = false;
		bool italic = false;
		bool bold = false;
		float padding = 0;
		Color textColor = { 1, 1, 1 };
		Color targetTextColor = textColor;
		Color textColorStart = { 1, 1, 1 };
		Color textColorEnd = { 1, 1, 1 };

		virtual void draw(float tx, float ty, bool editMode = false);
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual void revalidate();
		LabelWidget(GUI* gui, nlohmann::json j);
	};
}