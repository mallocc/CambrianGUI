#include "HLayout.h"
#include "GUI.h"

gui::HLayout::HLayout(GUI* gui) : Layout(gui) {}

bool gui::HLayout::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<HLayout>(ignoreType))
		{
		}
	}
	return true;
}

void gui::HLayout::revalidate()
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
			totalSpacing = 0.0;

		{
			float maxx = 0.0f;
			float maxy = 0.0f;
			float minx = getGUI()->w;
			float miny = getGUI()->h;
			for (Widget* widget : visibleChildren)
			{
				if (widget != nullptr)
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
				newHeight = (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxh : std::fabs(maxy - miny)) + totalPadding;
		}

		{
			float size = 0.0f;
			for (Widget* widget : visibleChildren)
				if (widget != nullptr)
					size += widget->W();
			float sizeOfContents = (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxw : size + totalSpacing) + totalPadding;
			float midy = newHeight / 2.0f;
			float midx = newWidth / 2.0f;
			if (isExpand(ExpandFlags::EXPAND_PREFERED_WIDTH))
				midx = sizeOfContents / 2.0f;

			{
				float emptySpace = 0.0f;
				if (!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
					emptySpace = std::fabs(newWidth - sizeOfContents);
				float dEmptySpace = 0.0f;
				if (visibleChildren.size() > 1)
					dEmptySpace = emptySpace / (float)(visibleChildren.size() - 1);

				float x = getPadding();

				if (isAlign(AlignFlags::ALIGN_FLAGS_RIGHT))
				{
					if (!isExpand(ExpandFlags::EXPAND_PREFERED_WIDTH))
					{
						if (!(!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) && isAlign(AlignFlags::ALIGN_FLAGS_SPACED)))
							x = newWidth - (size + totalSpacing + getPadding());
					}
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_LEFT))
				{
					x = getPadding();
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER) && !isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
				{
					x = midx - (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? maxw / 2.0f : (size + totalSpacing) / 2.0f);
				}

				for (auto& child : visibleChildren)
				{
					if (child != nullptr)
					{
						float y = getPadding();
						if (isAlign(AlignFlags::ALIGN_FLAGS_BOTTOM))
						{
							y = newHeight - child->H() - getPadding();
						}
						else if (isAlign(AlignFlags::ALIGN_FLAGS_TOP))
						{
							y = getPadding();
						}
						else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
						{
							y = midy - child->H() / 2.0f;
						}

						if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_Y_MASK)
							child->setY(y, FORCE);

						if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_X_MASK)
							child->setX(x - (child->isCentered() ? (child->W() / 2.0f) : 0.0f), FORCE);

						if (!isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
						{
							x += child->W();
							if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
								x += dEmptySpace;
							if (!isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
								x += getSpacing();
						}
					}
				}
			}
		}
		Layout::expand();
	}
}

float gui::HLayout::getPreferedWidth(Widget* child)
{
	if (child == nullptr || !isChild(child))
		return 0.0f;

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	if (visibleChildren.size() > 0)
	{
		float totalPadding = getPadding() * 2.0f;
		float totalSpacing = getSpacing() * (visibleChildren.size() - 1);
		float newWidth = W();
		float maxw = 0.0f;
		if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
			totalSpacing = 0.0;

		float maxx = 0.0f;
		float minx = getGUI()->w;
		for (Widget* widget : visibleChildren)
		{
			if (widget != nullptr)
				if (!widget->isOmitFromLayout())
				{
					maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
					minx = std::min(minx, (float)widget->X());
					maxw = std::max(maxw, (float)widget->W());
				}
		}

		if (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE))
			return maxw;

		if (isExpand(ExpandFlags::EXPAND_PREFERED_WIDTH))
			newWidth = std::fabs(maxx - minx) + totalPadding;

		int inflatedChildren = 1;
		float size = 0.0f;
		for (auto& c : getVisibleChildren())
			if (c != nullptr && !c->isOmitFromLayout() && c != child)
			{
				bool inflated = false;
				widget_as(Layout, layoutWidget, child)
				{
					if (layoutWidget->isExpand(ExpandFlags::EXPAND_INFLATE_WIDTH))
					{
						inflated = true;
					}
				}
				if (!inflated)
					size += c->W();
				else
					++inflatedChildren;
			}
		float sumOfWidths = size + totalSpacing;

		return ((newWidth - totalPadding) - sumOfWidths) / (float)inflatedChildren;
	}

	return Layout::getPreferedWidth(child);
}

