#include "HLayout.h"
#include "GUI.h"

gui::HLayout::HLayout(GUI* gui) : Layout(gui) {}

bool gui::HLayout::init(nlohmann::json j, bool ignoreType)
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
		float totalWeight = getTotalWeightOfChildren();
		float midy = H() / 2.0f;
		float midx = W() / 2.0f;
		float sizeOfContents = getSpacing() * (visibleChildren.size() - 1) + getPadding() * 2.0;
		float maxy = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			sizeOfContents += widget->W();
			maxy = std::max(maxy, (float)widget->Y());
		}

		{
			float emptySpace = std::fabs(W() - sizeOfContents);
			float dEmptySpace = emptySpace / (float)(visibleChildren.size() - 1);

			float x = getPadding();

			if (isAlign(AlignFlags::ALIGN_FLAGS_RIGHT))
			{
				x = W() - sizeOfContents + getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_LEFT))
			{
				x = getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
			{
				x = midx - (isAlign(AlignFlags::ALIGN_FLAGS_COLLAPSE) ? 0.0f : sizeOfContents / 2.0f);
			}

			size_t childCount = 0;
			for (auto& child : visibleChildren)
			{
				childCount++;

				float y = getPadding();
				if (isAlign(AlignFlags::ALIGN_FLAGS_BOTTOM))
				{
					y = H() - child->H() + getPadding();
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
					if (childCount < visibleChildren.size())
						x += getSpacing();
					if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
					{
						x += dEmptySpace;
					}
				}
			}
		}
	}

	expand();
}
