#include "Layout.h"
#include "GUI.h"
#include <GL/glew.h>

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
				fields["align"] = {
					"none",
							[&](std::string value)
					{
						for (int i = 0; i < ALIGNMENT::ALIGN_NUMBER; ++i)
							if (ALIGN_STRINGS[i] == value)
								alignment = (ALIGNMENT)i;
						//std::cout << "DEBUG: id=" << getId() << " value=" << value << " alignment=" << alignment << std::endl;
				},
							[&](std::string fieldName) { return nlohmann::json({{fieldName, ALIGN_STRINGS[alignment]}}); }
				};
				fields["size"] = {
					"inherit",
							[&](std::string value)
					{
						for (int i = 0; i < SIZING::SIZE_NUMBER; ++i)
							if (SIZE_STRINGS[i] == value)
								sizing = (SIZING)i;
						//std::cout << "DEBUG: id=" << getId() <<" value=" << value << " sizing=" << sizing << std::endl;
				},
							[&](std::string fieldName) { return nlohmann::json({{fieldName, SIZE_STRINGS[sizing]}}); }
				};
				fields["spacing"] = spacing;
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

	if (sizing == SIZING::SIZE_INHERIT)
	{
		// No point?
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
	switch (sizing)
	{
	case SIZE_EXPAND:
	{
		float maxx = 0.0f;
		float maxy = 0.0f;
		float minx = getGUI()->w;
		float miny = getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				minx = std::min(minx, (float)widget->X());
				miny = std::min(miny, (float)widget->Y());
			}
		}
		setW(maxx - minx, FORCE);
		setH(maxy - miny, FORCE);
		break;
	}
	case SIZE_EXPAND_WIDTH:
	{
		float maxx = 0.0f;
		float minx = getGUI()->w;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
				minx = std::min(minx, (float)widget->X());
			}
		}

		setW(maxx - minx, FORCE);
		if (getParent() != nullptr)
		{
			setH(getParent()->H(), FORCE);
		}
		else
		{
			setH(getGUI()->h, FORCE);
		}
		break;
	}
	case SIZE_EXPAND_HEIGHT:
	{
		float maxy = 0.0f;
		float miny = getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				miny = std::min(miny, (float)widget->Y());
			}
		}

		if (getParent() != nullptr)
		{
			setW(getParent()->W(), FORCE);
		}
		else
		{
			setW(getGUI()->w, FORCE);
		}
		setH(maxy - miny, FORCE);
		break;
	}
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

void gui::Layout::setSizing(SIZING sizing)
{
	this->sizing = sizing;
}

gui::SIZING gui::Layout::getSizing()
{
	return sizing;
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
