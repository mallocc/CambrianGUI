#include "VLayout.h"
#include <algorithm>

gui::VLayout::VLayout(GUI* gui, nlohmann::json j) : Layout(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

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
		float midx = W() / 2.0f;
		float midy = H() / 2.0f;
		float size = 0.0f;
		float maxx = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->H() + widget->getWeight() * (widget->isProportional() ? H() : 1);
			maxx = std::max(maxx, (float)widget->W());
		}

		switch (getAlignment())
		{
		case ALIGNMENT::ALIGN_LIST:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_CENTERED:
		{
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(midx - visibleChildren[i]->W() / 2.0f, FORCE);
				visibleChildren[i]->setY(visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1) - (visibleChildren[i]->isCentered() * visibleChildren[i]->H() / 2.0f), FORCE);
			}
			break;
		}
		case ALIGNMENT::ALIGN_TOP:
		{
			float start = getPadding();
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(midx - visibleChildren[i]->W() / 2.0f, FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_BOTTOM:
		{
			float start = H() - size - getPadding();
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(midx - visibleChildren[i]->W() / 2.0f, FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_LEFT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(getPadding(), FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RIGHT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(W() - visibleChildren[i]->W(), FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_STACKED:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(midx - visibleChildren[i]->W() / 2.0f, FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_START:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setY(start, FORCE);
				visibleChildren[i]->setX(0.0f, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_END:
		{
			float leftOver = H() - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(W() - visibleChildren[i]->W(), FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RSTART:
		{
			float leftOver = H() - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setY(start, FORCE);
				visibleChildren[i]->setX(0.0f, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_REND:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(W() - visibleChildren[i]->W(), FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_SPACED:
		{
			float leftOver = H() - size - getPadding() * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = getPadding();
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->setX(midx - visibleChildren[i]->W() / 2.0f, FORCE);
				visibleChildren[i]->setY(start, FORCE);
				start += visibleChildren[i]->H() + d + visibleChildren[i]->getWeight() * (visibleChildren[i]->isProportional() ? H() : 1);
				if (visibleChildren[i]->isCentered())
					start -= visibleChildren[i]->H() / 2.0f;
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