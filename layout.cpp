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
				fields["expand"] = { "default",
					[&](std::string value)
					{
						if (!nlohmann::json::accept(value))
						{
							expandFlags = ExpandFlags::EXPAND_RESET;
							if (EXPAND_FLAGS.contains(value))
							{
								expandFlags = EXPAND_FLAGS.at(value);
							}
						}
						else
						{
							nlohmann::json flagsJson = nlohmann::json::parse(value);
							if (flagsJson.is_array())
							{
								expandFlags = ExpandFlags::EXPAND_RESET;
								for (auto& flagJson : flagsJson)
								{
									if (EXPAND_FLAGS.contains(flagJson))
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
	for (auto& widget : visibleChildren)
	{
		if (widget != nullptr)
		widget->revalidate();
	}

	expand();
}

void gui::Layout::expand()
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	float totalPadding = getPadding() * 2.0f;
	float totalSpacing = getSpacing() * (visibleChildren.size() - 1);
	{
		float maxx = 0.0f;
		float maxy = 0.0f;
		float minx = getGUI()->w;
		float miny = getGUI()->h;
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
			setW(std::fabs(maxx - minx) + totalPadding, FORCE);
		if (isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
			setH(std::fabs(maxy - miny) + totalPadding, FORCE);
	}
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
	return W() - getPadding() * 2.0f;
}

float gui::Layout::getPreferedHeight(Widget* child)
{
	return H() - getPadding() * 2.0f;
}