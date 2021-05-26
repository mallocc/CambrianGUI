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
		float midx = W() / 2.0f;
		float midy = H() / 2.0f;
		float size = getPadding() * 2.0f;
		float maxy = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->W();
			maxy = std::max(maxy, (float)widget->H());
		}

		{
			float emptySpace = std::fabs(W() - size);
			float dEmptySpace = emptySpace / (float)(visibleChildren.size() - 1);

			float x = getPadding();
			if (isAlign(AlignFlags::ALIGN_FLAGS_RIGHT))
			{
				x = W() - size - getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_LEFT))
			{
				x = getPadding();
			}
			else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
			{
				x = midx - size / 2.0f;
			}

			for (auto& child : visibleChildren)
			{
				float y = getPadding();

				if (isAlign(AlignFlags::ALIGN_FLAGS_BOTTOM))
				{
					y = H() - child->H() - getPadding();
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_TOP))
				{
					y = getPadding();
				}
				else if (isAlign(AlignFlags::ALIGN_FLAGS_CENTER))
				{
					y = midy - child->H() / 2.0f;
				}

				if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_X_MASK)
					child->setX(x, FORCE);
				if (getAlignFlags() & AlignFlags::ALIGN_FLAGS_Y_MASK)
					child->setY(y, FORCE);

				//if (isAlign(AlignFlags::ALIGN_FLAGS_STACKED))
				{
					x += child->W() / (child->isCentered() ? 2.0f : 1.0f);
				}
				if (isAlign(AlignFlags::ALIGN_FLAGS_SPACED))
				{
					x += dEmptySpace;
				}

			}
		}


		if (false)
			switch (getAlignment())
			{
			case ALIGNMENT::ALIGN_LIST:
			{
				float start = 0.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_CENTERED:
			{
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(midx - visibleChildren[i]->H() / 2.0f, FORCE);
				}
				break;
			}
			case ALIGNMENT::ALIGN_LEFT:
			{
				float start = 0;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(midy - visibleChildren[i]->H() / 2.0f, FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_RIGHT:
			{
				float start = W() - size;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(midy - visibleChildren[i]->H() / 2.0f, FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + +visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_TOP:
			{
				float start = midx - size / 2.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(0, FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_BOTTOM:
			{
				float start = midx - size / 2.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(H() - visibleChildren[i]->H(), FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_STACKED:
			{
				float start = midx - size / 2.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(midy - visibleChildren[i]->H() / 2.0f, FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_START:
			{
				float start = 0.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setX(start, FORCE);
					visibleChildren[i]->setY(0.0f, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_END:
			{
				float leftOver = W() - size;
				float start = leftOver;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(H() - visibleChildren[i]->H(), FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_RSTART:
			{
				float leftOver = W() - size;
				float start = leftOver;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setX(start, FORCE);
					visibleChildren[i]->setY(0.0f, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_REND:
			{
				float start = 0.0f;
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(H() - visibleChildren[i]->H(), FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_SPACED:
			{
				float leftOver = W() - size - getPadding() * 2.0f;
				float d = leftOver / (visibleChildren.size() - 1);
				float start = getPadding();
				for (int i = 0; i < visibleChildren.size(); ++i)
				{
					visibleChildren[i]->setY(midy - visibleChildren[i]->H() / 2.0f, FORCE);
					visibleChildren[i]->setX(start, FORCE);
					start += visibleChildren[i]->W() + d + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? W() : 1);
					if (visibleChildren[i]->isCentered())
						start -= visibleChildren[i]->W() / 2.0f;
				}
				break;
			}
			case ALIGNMENT::ALIGN_NONE:
			default:
				break;
			}
	}

	expand();
}

float gui::HLayout::getPreferedHeight(Widget* child)
{
	return H();
}
