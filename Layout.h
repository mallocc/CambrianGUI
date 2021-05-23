#pragma once

#include "Container.h"

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

	class Layout : public Container, public WidgetType<Layout>
	{
		DEFINE_WIDGET_TYPE("layout");
	public:
		
		virtual bool init(nlohmann::json j, bool ignoreType = false);		

		virtual void revalidate();

		virtual void expand();
	
		Layout(GUI* gui);
		
		void setAlignment(ALIGNMENT alignment);
		ALIGNMENT getAlignment();

		void setSizing(SIZING sizing);
		SIZING getSizing();

		void setPadding(float padding);
		float getPadding();

		void setSpacing(float spacing);
		float getSpacing();


	private:

		ALIGNMENT alignment = ALIGNMENT::ALIGN_NONE;
		SIZING sizing = SIZING::SIZE_INHERIT;
		float padding = 0.0f;
		float spacing = 0.0f;
	};
}