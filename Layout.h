#pragma once

#include "Container.h"
#include <bit>

namespace gui
{
	enum AlignFlags
	{
		ALIGN_FLAGS_RESET	= 0b00000000,
		ALIGN_FLAGS_TOP		= 0b00000001,
		ALIGN_FLAGS_BOTTOM	= 0b00000010,
		ALIGN_FLAGS_LEFT	= 0b00000100,
		ALIGN_FLAGS_RIGHT	= 0b00001000,
		ALIGN_FLAGS_CENTER	= 0b00010000,
		ALIGN_FLAGS_COLLAPSE = 0b00100000,
		ALIGN_FLAGS_SPACED	= 0b01000000,
		ALIGN_FLAGS_INFLATE	= 0b10000000,
		ALIGN_FLAGS_X_MASK	= ALIGN_FLAGS_LEFT | ALIGN_FLAGS_RIGHT | ALIGN_FLAGS_CENTER,
		ALIGN_FLAGS_Y_MASK	= ALIGN_FLAGS_TOP | ALIGN_FLAGS_BOTTOM | ALIGN_FLAGS_CENTER,
		ALIGN_FLAGS_LIST	= ALIGN_FLAGS_TOP,
		ALIGN_FLAGS_START	= ALIGN_FLAGS_TOP | ALIGN_FLAGS_LEFT,
		ALIGN_FLAGS_END		= ALIGN_FLAGS_BOTTOM | ALIGN_FLAGS_RIGHT,
		ALIGN_FLAGS_DEFAULT = ALIGN_FLAGS_RESET,
	};

	const static std::map<std::string, AlignFlags> ALIGN_FLAGS = {
		{ "reset", AlignFlags::ALIGN_FLAGS_RESET },
		{ "none", AlignFlags::ALIGN_FLAGS_RESET },
		{ "top", AlignFlags::ALIGN_FLAGS_TOP },
		{ "bottom", AlignFlags::ALIGN_FLAGS_BOTTOM },
		{ "left", AlignFlags::ALIGN_FLAGS_LEFT },
		{ "right", AlignFlags::ALIGN_FLAGS_RIGHT },
		{ "center", AlignFlags::ALIGN_FLAGS_CENTER },
		{ "collapse", AlignFlags::ALIGN_FLAGS_COLLAPSE },
		{ "spaced", AlignFlags::ALIGN_FLAGS_SPACED },
		{ "inflate", AlignFlags::ALIGN_FLAGS_INFLATE },
		{ "list", AlignFlags::ALIGN_FLAGS_LIST },
		{ "start", AlignFlags::ALIGN_FLAGS_START },
		{ "end", AlignFlags::ALIGN_FLAGS_END },
		{ "default", AlignFlags::ALIGN_FLAGS_LIST }
	};


	enum ExpandFlags
	{
		EXPAND_RESET				= 0b000000000,
		EXPAND_PREFERED_WIDTH		= 0b000000001,
		EXPAND_FILL_WIDTH			= 0b000000010,
		EXPAND_INFLATE_WIDTH		= 0b000000100,
		EXPAND_WIDTH_MASK			= EXPAND_PREFERED_WIDTH | EXPAND_FILL_WIDTH | EXPAND_INFLATE_WIDTH,
		EXPAND_PREFERED_HEIGHT		= 0b000001000,
		EXPAND_FILL_HEIGHT			= 0b000010000,
		EXPAND_INFLATE_HEIGHT		= 0b000100000,
		EXPAND_HEIGHT_MASK			= EXPAND_PREFERED_HEIGHT | EXPAND_FILL_HEIGHT | EXPAND_INFLATE_HEIGHT,
		EXPAND_PREFERED				= EXPAND_PREFERED_WIDTH | EXPAND_PREFERED_HEIGHT,
		EXPAND_FILL					= EXPAND_FILL_WIDTH | EXPAND_FILL_HEIGHT,
		EXPAND_INFLATE				= EXPAND_INFLATE_WIDTH | EXPAND_INFLATE_HEIGHT,
		EXPAND_DEFAULT				= EXPAND_RESET,
	};

	const static std::map<std::string, ExpandFlags> EXPAND_FLAGS = {
		{ "reset", ExpandFlags::EXPAND_RESET },
		{ "none", ExpandFlags::EXPAND_RESET },
		{ "prefered-width", ExpandFlags::EXPAND_PREFERED_WIDTH },
		{ "prefered-height", ExpandFlags::EXPAND_PREFERED_HEIGHT },
		{ "fill-width", ExpandFlags::EXPAND_FILL_WIDTH },
		{ "fill-height", ExpandFlags::EXPAND_FILL_HEIGHT },
		{ "inflate-width", ExpandFlags::EXPAND_INFLATE_WIDTH },
		{ "inflate-height", ExpandFlags::EXPAND_INFLATE_HEIGHT },
		{ "prefered", ExpandFlags::EXPAND_PREFERED },
		{ "fill", ExpandFlags::EXPAND_FILL },
		{ "inflate", ExpandFlags::EXPAND_INFLATE },
		{ "default", ExpandFlags::EXPAND_DEFAULT }
	};


	class Layout : public Container, public WidgetType<Layout>
	{
		DEFINE_WIDGET_TYPE("layout");
	public:

		virtual bool init(nlohmann::json j, bool ignoreType = false);

		virtual void revalidate();

		virtual void expand();

		Layout(GUI* gui);

		void setPadding(float padding);
		float getPadding();

		void setSpacing(float spacing);
		float getSpacing();

		void setExpandFlags(uint8_t flags);
		uint8_t getExpandFlags();
		void setAlignFlags(uint8_t flags);
		uint8_t getAlignFlags();


		bool isAlign(uint8_t flag);
		bool isExpand(uint8_t flag);

		virtual float getPreferedWidth(Widget* child);
		virtual float getPreferedHeight(Widget* child);

	private:

		uint8_t expandFlags = ExpandFlags::EXPAND_DEFAULT;
		uint8_t alignFlags = AlignFlags::ALIGN_FLAGS_DEFAULT;
		float padding = 0.0f;
		float spacing = 0.0f;
	};
}