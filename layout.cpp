#include "Layout.h"
#include "GUI.h"
#include <GL/glew.h>
#include "Utilities.h"
#include <bitset>

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
				fields["alignment"] = { "none",
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
						if (!nlohmann::json::accept(value))
						{
							if (EXPAND_FLAGS.contains(value))
								if (EXPAND_FLAGS.at(value) == ExpandFlags::EXPAND_RESET)
								{
									expandFlags = ExpandFlags::EXPAND_RESET;
								}
								else
								{
									uint8_t flag = EXPAND_FLAGS.at(value);
									if ((flag & ExpandFlags::EXPAND_WIDTH_MASK))
										expandFlags = (expandFlags & ~ExpandFlags::EXPAND_WIDTH_MASK) | (flag & ExpandFlags::EXPAND_WIDTH_MASK);
									if ((flag & ExpandFlags::EXPAND_HEIGHT_MASK))
										expandFlags = (expandFlags & ~ExpandFlags::EXPAND_HEIGHT_MASK) | (flag & ExpandFlags::EXPAND_HEIGHT_MASK);
								}
						}
						else if (false)
						{
							nlohmann::json flagsJson = nlohmann::json::parse(value);
							if (flagsJson.is_array())
							{
								for (auto& flagJson : flagsJson)
								{
									if (EXPAND_FLAGS.contains(flagJson))
										if (EXPAND_FLAGS.at(flagJson) == ExpandFlags::EXPAND_RESET)
										{
											expandFlags = ExpandFlags::EXPAND_RESET;
										}
										else
										{
											uint8_t flag = EXPAND_FLAGS.at(flagJson);
											if ((flag & ExpandFlags::EXPAND_WIDTH_MASK))
												expandFlags |= (expandFlags & ~ExpandFlags::EXPAND_WIDTH_MASK) | (flag & ExpandFlags::EXPAND_WIDTH_MASK);
											if ((flag & ExpandFlags::EXPAND_HEIGHT_MASK))
												expandFlags |= (expandFlags & ~ExpandFlags::EXPAND_HEIGHT_MASK) | (flag & ExpandFlags::EXPAND_HEIGHT_MASK);
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
				fields["align"] = { "default",
					[&](std::string value)
					{
						if (!nlohmann::json::accept(value))
						{
							alignFlags = AlignFlags::ALIGN_FLAGS_RESET;
							if (ALIGN_FLAGS.contains(value))
							{
								alignFlags = ALIGN_FLAGS.at(value);
							}
						}
						else
						{
							nlohmann::json flagsJson = nlohmann::json::parse(value);
							if (flagsJson.is_array())
							{
								alignFlags = AlignFlags::ALIGN_FLAGS_RESET;
								for (auto& flag : flagsJson)
								{
									if (ALIGN_FLAGS.contains(flag))
									{
										alignFlags |= ALIGN_FLAGS.at(flag);
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

			//std::cout << "Align: " << std::bitset<8>(alignFlags) << " expand: " << std::bitset<8>(expandFlags) << " inflate? " << isExpand(ExpandFlags::EXPAND_INFLATE_WIDTH) << std::endl;

			m_config += fields;
		}
	}

	return true;
}

void gui::Layout::revalidate()
{
	Widget::revalidate();

	if (isExpand(ExpandFlags::EXPAND_FILL_WIDTH))
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
	if (isExpand(ExpandFlags::EXPAND_FILL_HEIGHT))
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

	if (isExpand(ExpandFlags::EXPAND_INFLATE_WIDTH))
	{
		widget_as(Layout, parentLayout, getParent())
		{
			setW(parentLayout->getPreferedWidth(this), FORCE);
			//std::cout << "inflate width: " << W() << std::endl;
		}
	}
	if (isExpand(ExpandFlags::EXPAND_INFLATE_HEIGHT))
	{
		widget_as(Layout, parentLayout, getParent())
		{
			setH(parentLayout->getPreferedHeight(this), FORCE);
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
		if (isExpand(ExpandFlags::EXPAND_PREFERED_WIDTH))
			setW(maxx - minx, FORCE);
		if (isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
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

uint8_t gui::Layout::getAlignFlags()
{
	return alignFlags;
}


bool gui::Layout::isAlign(uint8_t flag)
{
	return (alignFlags & flag) == flag;
}

bool gui::Layout::isExpand(uint8_t flag)
{
	return (expandFlags & flag) == flag;
}

float gui::Layout::getPreferedWidth(Widget* child)
{
	if (child == nullptr || !isChild(child))
		return 0.0f;

	float width = 0.0f;
	for (auto& c : getVisibleChildren())
		if (!c->isOmitFromLayout() && c != child)
		{
			width += c->W();
		}

	return W() - width;
}

float gui::Layout::getPreferedHeight(Widget* child)
{
	if (child == nullptr || !isChild(child))
		return 0.0f;

	float height = 0.0f;
	for (auto& c : getVisibleChildren())
		if (!c->isOmitFromLayout() && c != child)
		{
			height += c->H();
		}

	return H() - height;
}