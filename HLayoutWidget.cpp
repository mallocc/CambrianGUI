#include "HLayoutWidget.h"
#include "GUI.h"

gui::HLayoutWidget::HLayoutWidget(GUI* gui, nlohmann::json j)
	: ContainerWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

bool gui::HLayoutWidget::init(nlohmann::json j, bool ignoreType)
{
	if (ContainerWidget::init(j, true))
	{
		if (checkWidgetType<HLayoutWidget>(ignoreType))
		{
		}
	}
	return true;
}

void gui::HLayoutWidget::revalidate()
{
	ContainerWidget::revalidate();

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	if (visibleChildren.size() > 0)
	{
		float midx = W() / 2.0f;
		float midy = H() / 2.0f;
		float size = 0.0f;
		float maxy = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->W() + widget->getWeight() * (widget->isProportional() ? W() : 1);
			maxy = std::max(maxy, (float)widget->H());
		}

		switch (alignment)
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
			float leftOver = W() - size - padding * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = padding;
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