#pragma once

#include "Container.h"
#include <bit>

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

	enum AlignFlags
	{
		ALIGN_FLAGS_RESET	= 0b00000000,
		ALIGN_FLAGS_TOP		= 0b00000001,
		ALIGN_FLAGS_BOTTOM	= 0b00000010,
		ALIGN_FLAGS_LEFT	= 0b00000100,
		ALIGN_FLAGS_RIGHT	= 0b00001000,
		ALIGN_FLAGS_CENTER	= 0b00010000,
		ALIGN_FLAGS_STACKED = 0b00100000,
		ALIGN_FLAGS_SPACED	= 0b01000000,
		ALIGN_FLAGS_LIST	= ALIGN_FLAGS_TOP | ALIGN_FLAGS_CENTER | ALIGN_FLAGS_STACKED,
		ALIGN_FLAGS_START	= ALIGN_FLAGS_TOP | ALIGN_FLAGS_LEFT,
		ALIGN_FLAGS_END		= ALIGN_FLAGS_BOTTOM | ALIGN_FLAGS_RIGHT,
		ALIGN_FLAGS_DEFAULT = ALIGN_FLAGS_CENTER | ALIGN_FLAGS_STACKED,
	};

	const static std::map<std::string, AlignFlags> ALIGN_FLAGS = {
		{ "reset", AlignFlags::ALIGN_FLAGS_RESET },
		{ "none", AlignFlags::ALIGN_FLAGS_RESET },
		{ "top", AlignFlags::ALIGN_FLAGS_TOP },
		{ "bottom", AlignFlags::ALIGN_FLAGS_BOTTOM },
		{ "left", AlignFlags::ALIGN_FLAGS_LEFT },
		{ "right", AlignFlags::ALIGN_FLAGS_RIGHT },
		{ "center", AlignFlags::ALIGN_FLAGS_CENTER },
		{ "stacked", AlignFlags::ALIGN_FLAGS_STACKED },
		{ "spaced", AlignFlags::ALIGN_FLAGS_SPACED },
		{ "list", AlignFlags::ALIGN_FLAGS_LIST },
		{ "start", AlignFlags::ALIGN_FLAGS_START },
		{ "end", AlignFlags::ALIGN_FLAGS_END },
		{ "default", AlignFlags::ALIGN_FLAGS_DEFAULT }
	};


	enum ExpandFlags
	{
		EXPAND_RESET = 0b000000000,
		EXPAND_PREFERED_WIDTH = 0b000000001,
		EXPAND_PREFERED_HEIGHT = 0b000000010,
		EXPAND_FILL_WIDTH = 0b000000100,
		EXPAND_FILL_HEIGHT = 0b000001000,
		EXPAND_PREFERED = EXPAND_PREFERED_WIDTH | EXPAND_PREFERED_HEIGHT,
		EXPAND_FILL = EXPAND_FILL_WIDTH | EXPAND_FILL_HEIGHT,
		EXPAND_DEFAULT = EXPAND_FILL,
	};

	const static std::map<std::string, ExpandFlags> EXPAND_FLAGS = {
		{ "reset", ExpandFlags::EXPAND_RESET },
		{ "none", ExpandFlags::EXPAND_RESET },
		{ "prefered-width", ExpandFlags::EXPAND_PREFERED_WIDTH },
		{ "prefered-height", ExpandFlags::EXPAND_PREFERED_HEIGHT },
		{ "fill-width", ExpandFlags::EXPAND_FILL_WIDTH },
		{ "fill-height", ExpandFlags::EXPAND_FILL_HEIGHT },
		{ "prefered", ExpandFlags::EXPAND_PREFERED },
		{ "fill", ExpandFlags::EXPAND_FILL },
		{ "default", ExpandFlags::EXPAND_DEFAULT }
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

		void setPadding(float padding);
		float getPadding();

		void setSpacing(float spacing);
		float getSpacing();

		void setExpandFlags(uint8_t flags);
		uint8_t getExpandFlags();
		void setAlignFlags(uint8_t flags);
		uint8_t setAlignFlags();

	private:

		ALIGNMENT alignment = ALIGNMENT::ALIGN_NONE;
		uint8_t expandFlags = ExpandFlags::EXPAND_DEFAULT;
		uint8_t alignFlags = AlignFlags::ALIGN_FLAGS_DEFAULT;
		float padding = 0.0f;
		float spacing = 0.0f;
	};
}