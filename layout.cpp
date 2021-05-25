#include "Layout.h"
#include "GUI.h"
#include <GL/glew.h>
#include "Utilities.h"

using gui::Layout;
using gui::Widget;

Layout::Layout(GUI* gui) : Container(gui) {}


bool gui::Layout::init(nlohmann::json j, bool ignoreType)
{
	if (Container::init(j, true))
	{
		if (checkWidgetType<Layout>(ignoreType))
		{
			ConfigList fields;
			{
				fields["padding"] = padding;
				fields["spacing"] = spacing;
				fields["align"] = { "none",
					[&](std::string value)
					{
						for (int i = 0; i < ALIGNMENT::ALIGN_NUMBER; ++i)
							if (ALIGN_STRINGS[i] == value)
								alignment = (ALIGNMENT)i;
					},
					[&](std::string fieldName) { return nlohmann::json({{fieldName, ALIGN_STRINGS[alignment]}}); }
				};
				fields["expand"] = { "default", 
					[&](std::string value)
					{
						nlohmann::json flagsJson;
						flagsJson["flags"] = value;
						json_get_object(flagsJson, "flags", flags)
						{
							if (flags.is_string())
							{
								if (EXPAND_FLAGS.contains(flags.get<std::string>()))
									if (EXPAND_FLAGS.at(flags.get<std::string>()) == ExpandFlags::EXPAND_RESET)
									{
										expandFlags = ExpandFlags::EXPAND_RESET;
									}
									else
									{
										expandFlags |= EXPAND_FLAGS.at(flags.get<std::string>());
									}
							}
							else if (flags.is_array())
							{
								for (auto& flag : flags)
								{
									if (EXPAND_FLAGS.contains(flags.get<std::string>()))
										if (EXPAND_FLAGS.at(flags.get<std::string>()) == ExpandFlags::EXPAND_RESET)
										{
											expandFlags = ExpandFlags::EXPAND_RESET;
										}
										else
										{
											expandFlags |= EXPAND_FLAGS.at(flags.get<std::string>());
										}
								}
							}
						}
					},
					[&](std::string fieldName) 
					{
						return nlohmann::json({{fieldName, "TODO"}});
					}
				};
				fields["alignment"] = { "default",
					[&](std::string value)
					{
						nlohmann::json flagsJson;
						flagsJson["flags"] = value;
						json_get_object(flagsJson, "flags", flags)
						{
							if (flags.is_string())
							{
								if (ALIGN_FLAGS.contains(flags.get<std::string>()))
									if (ALIGN_FLAGS.at(flags.get<std::string>()) == AlignFlags::ALIGN_FLAGS_RESET)
									{
										expandFlags = AlignFlags::ALIGN_FLAGS_RESET;
									}
									else
									{
										expandFlags |= ALIGN_FLAGS.at(flags.get<std::string>());
									}
							}
							else if (flags.is_array())
							{
								for (auto& flag : flags)
								{
									if (ALIGN_FLAGS.contains(flags.get<std::string>()))
										if (ALIGN_FLAGS.at(flags.get<std::string>()) == AlignFlags::ALIGN_FLAGS_RESET)
										{
											expandFlags = AlignFlags::ALIGN_FLAGS_RESET;
										}
										else
										{
											expandFlags |= ALIGN_FLAGS.at(flags.get<std::string>());
										}
								}
							}
						}
					},
					[&](std::string fieldName)
					{
						return nlohmann::json({{fieldName, "TODO"}});
					}
				};
			}
			fields.load(j);

			m_config += fields;
		}
	}

	return true;
}

void gui::Layout::revalidate()
{
	Widget::revalidate();

	//if (sizing == SIZING::SIZE_INHERIT)
	//{
	//	// No point?
	//	if (W() == 0)
	//	{
	//		if (getParent() != nullptr)
	//		{
	//			setW(getParent()->W(), FORCE);
	//		}
	//		else
	//		{
	//			setW(getGUI()->w, FORCE);
	//		}
	//	}
	//	if (H() == 0)
	//	{
	//		if (getParent() != nullptr)
	//		{
	//			setH(getParent()->H(), FORCE);
	//		}
	//		else
	//		{
	//			setH(getGUI()->h, FORCE);
	//		}
	//	}
	//}

	if ((getExpandFlags() & ExpandFlags::EXPAND_FILL_WIDTH) == ExpandFlags::EXPAND_FILL_WIDTH)
	{
		if (W() == 0)
		{
			if (getParent() != nullptr)
			{
				setW(getParent()->W(), FORCE);
			}
			else
			{
				setW(getGUI()->w, FORCE);
			}
		}
	}
	if ((getExpandFlags() & ExpandFlags::EXPAND_FILL_HEIGHT) == ExpandFlags::EXPAND_FILL_HEIGHT)
	{
		if (H() == 0)
		{
			if (getParent() != nullptr)
			{
				setH(getParent()->H(), FORCE);
			}
			else
			{
				setH(getGUI()->h, FORCE);
			}
		}
	}

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (Widget* widget : visibleChildren)
	{
		widget->revalidate();
	}

	expand();
}

void gui::Layout::expand()
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	//switch (sizing)
	//{
	//case SIZE_EXPAND:
	//{
	//	float maxx = 0.0f;
	//	float maxy = 0.0f;
	//	float minx = getParent() != nullptr ? getParent()->W() : getGUI()->w;
	//	float miny = getParent() != nullptr ? getParent()->H() : getGUI()->h;
	//	for (Widget* widget : visibleChildren)
	//	{
	//		if (!widget->isOmitFromLayout())
	//		{
	//			maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
	//			maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
	//			minx = std::min(minx, (float)widget->X());
	//			miny = std::min(miny, (float)widget->Y());
	//		}
	//	}
	//	setW(maxx - minx, FORCE);
	//	setH(maxy - miny, FORCE);
	//	break;
	//}
	//case SIZE_EXPAND_WIDTH:
	//{
	//	float maxx = 0.0f;
	//	float minx = getParent() != nullptr ? getParent()->W() : getGUI()->w;
	//	for (Widget* widget : visibleChildren)
	//	{
	//		if (!widget->isOmitFromLayout())
	//		{
	//			maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
	//			minx = std::min(minx, (float)widget->X());
	//		}
	//	}
	//	setW(maxx - minx, FORCE);
	//	//if (getParent() != nullptr)
	//	//{
	//	//	setH(getParent()->H(), FORCE);
	//	//}
	//	//else
	//	//{
	//	//	setH(getGUI()->h, FORCE);
	//	//}
	//	break;
	//}
	//case SIZE_EXPAND_HEIGHT:
	//{
	//	float maxy = 0.0f;
	//	float miny = getParent() != nullptr ? getParent()->H() : getGUI()->h;
	//	for (Widget* widget : visibleChildren)
	//	{
	//		if (!widget->isOmitFromLayout())
	//		{
	//			maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
	//			miny = std::min(miny, (float)widget->Y());
	//		}
	//	}
	//	//if (getParent() != nullptr)
	//	//{
	//	//	setW(getParent()->W(), FORCE);
	//	//}
	//	//else
	//	//{
	//	//	setW(getGUI()->w, FORCE);
	//	//}
	//	setH(maxy - miny, FORCE);
	//	break;
	//}
	//}

	{
		float maxx = 0.0f;
		float maxy = 0.0f;
		float minx = getParent() != nullptr ? getParent()->W() : getGUI()->w;
		float miny = getParent() != nullptr ? getParent()->H() : getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isOmitFromLayout())
			{
				maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				minx = std::min(minx, (float)widget->X());
				miny = std::min(miny, (float)widget->Y());
			}
		}
		if ((getExpandFlags() & ExpandFlags::EXPAND_PREFERED_WIDTH) == ExpandFlags::EXPAND_PREFERED_WIDTH)
			setW(maxx - minx, FORCE);
		if ((getExpandFlags() & ExpandFlags::EXPAND_PREFERED_HEIGHT) == ExpandFlags::EXPAND_PREFERED_HEIGHT)
			setH(maxy - miny, FORCE);
	}	
}

void gui::Layout::setAlignment(ALIGNMENT alignment)
{
	this->alignment = alignment;
}

gui::ALIGNMENT gui::Layout::getAlignment()
{
	return alignment;
}

void gui::Layout::setPadding(float padding)
{
	this->padding = padding;
}

float gui::Layout::getPadding()
{
	return padding;
}

void gui::Layout::setSpacing(float spacing)
{
	this->spacing = spacing;
}

float gui::Layout::getSpacing()
{
	return spacing;
}

void gui::Layout::setExpandFlags(uint8_t flags)
{
	this->expandFlags = flags;
}

uint8_t gui::Layout::getExpandFlags()
{
	return this->expandFlags;
}
