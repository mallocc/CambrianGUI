#include "VLayout.h"
#include <algorithm>

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
		float totalWeight = getTotalWeightOfChildren();
		float midx = W() / 2.0f;
		float midy = H() / 2.0f;
		float sizeOfContents = getSpacing() * (visibleChildren.size() - 1) + getPadding() * 2.0;
		float maxx = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			sizeOfContents += widget->H();
			maxx = std::max(maxx, (float)widget->W());
		}

		{
			float emptySpace = std::fabs(H() - sizeOfContents);
			float dEmptySpace = emptySpace / (float)(visibleChildren.size() - 1);

			float y = getPadding();

			if (isAlign(AlignFlags::ALIGN_FLAGS_BOTTOM))
			{
				y = H() - sizeOfContents + getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_TOP))
			{
				y = getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
			{
				y = midy - (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? 0.0f : sizeOfContents / 2.0f);
			}

			size_t childCount = 0;
			for (auto& child : visibleChildren)
			{
				childCount++;

				float x = getPadding();
				if (isAlign(AlignFlags::ALIGN_FLAGS_RIGHT))
				{
					x = W() - child->W() + getPadding();
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
					if (childCount < visibleChildren.size())
						y += getSpacing();
					if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
					{
						y += dEmptySpace;
					}
				}
			}
		}
	}

	expand();
}
