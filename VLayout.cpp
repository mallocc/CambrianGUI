#include "VLayout.h"
#include "GUI.h"

gui::VLayout::VLayout(GUI* gui) : Layout(gui) {}

bool gui::VLayout::init(nlohmann::json j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<VLayout>(ignoreType))
		{
		}
	}
	return true;
}

void gui::VLayout::revalidate()
{
	Layout::revalidate();

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	if (visibleChildren.size() > 0)
	{
		float newWidth = W();
		float newHeight = H();
		float maxw = 0.0f;
		float maxh = 0.0f;
		float totalPadding = getPadding() * 2.0f;
		float totalSpacing = getSpacing() * (visibleChildren.size() - 1);
		if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
			totalSpacing = 0.0f;

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
					maxw = std::max(maxw, (float)widget->W());
					maxh = std::max(maxh, (float)widget->H());
				}
			}
			if (isExpand(ExpandFlags::EXPAND_PREFERED_WIDTH))
				newWidth = (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxw : std::fabs(maxx - minx)) + totalPadding;
			if (isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
				newHeight = (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxh : std::fabs(maxy - miny) + totalSpacing) + totalPadding;
		}

		{
			float size = 0.0f;
			for (Widget* widget : visibleChildren)
				size += widget->H();
			float sizeOfContents = (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxh : size + totalSpacing) + totalPadding;
			float midx = newWidth / 2.0f;
			float midy = newHeight / 2.0f;
			if (isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
				midy = sizeOfContents / 2.0f;

			{
				float emptySpace = 0.0f;
				if (!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
					emptySpace = std::fabs(newHeight - sizeOfContents);
				float dEmptySpace = 0.0f;
				if (visibleChildren.size() > 1)
					dEmptySpace = emptySpace / (float)(visibleChildren.size() - 1);

				float y = getPadding();

				if (isAlign(AlignFlags::ALIGN_FLAGS_BOTTOM))
				{
					if (!isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
					{
						if (!(!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) && isAlign(AlignFlags::ALIGN_FLAGS_SPACED)))
							y = newHeight - (size + totalSpacing + getPadding());
					}
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_TOP))
				{
					y = getPadding();
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER) && !isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
				{
					y = midy - (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxh / 2.0f : (size + totalSpacing) / 2.0f);
				}

				for (auto& child : visibleChildren)
				{
					float x = getPadding();
					if (isAlign(AlignFlags::ALIGN_FLAGS_RIGHT))
					{
						x = newWidth - child->W() - getPadding();
					}
					else if (isAlign(AlignFlags::ALIGN_FLAGS_LEFT))
					{
						x = getPadding();
					}
					else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
					{
						x = midx - child->W() / 2.0f;
					}

					if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_X_MASK)
						child->setX(x, FORCE);

					if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_Y_MASK)
						child->setY(y - (child->isCentered() ? (child->H() / 2.0f) : 0.0f), FORCE);

					if (!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
					{
						y += child->H();
						y += dEmptySpace;
						if (!isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
							y += getSpacing();
					}
				}
			}
		}
		Layout::expand();
	}
}

float gui::VLayout::getPreferedHeight(Widget* child)
{
	if (child == nullptr || !isChild(child))
		return 0.0f;

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	if (visibleChildren.size() > 0)
	{
		float totalPadding = getPadding() * 2.0f;
		float totalSpacing = getSpacing() * (visibleChildren.size() - 1);
		float newHeight = H();
		float maxh = 0.0f;

		if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
			totalSpacing = 0.0f;

		float maxy = 0.0f;
		float miny = getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isOmitFromLayout())
			{
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				miny = std::min(miny, (float)widget->Y());
				maxh = std::max(maxh, (float)widget->H());
			}
		}

		if (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
			return maxh;

		if (isExpand(ExpandFlags::EXPAND_PREFERED_HEIGHT))
			newHeight = std::fabs(maxy - miny) + totalPadding;

		int inflatedChildren = 1;
		float size = 0.0f;
		for (auto& c : getVisibleChildren())
			if (!c->isOmitFromLayout() && c != child)
			{
				bool inflated = false;
				widget_as(Layout, layoutWidget, child)
				{
					if (layoutWidget->isExpand(ExpandFlags::EXPAND_INFLATE_HEIGHT))
					{
						inflated = true;
					}
				}
				if (!inflated)
					size += c->H();
				else
					++inflatedChildren;
			}
		float sumOfHeights = size + totalSpacing;
		
		return ((newHeight - totalPadding) - sumOfHeights) / (float)inflatedChildren;
	}

	return Layout::getPreferedHeight(child);
}

