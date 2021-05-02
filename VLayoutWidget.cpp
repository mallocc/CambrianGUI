#include "VLayoutWidget.h"
#include <algorithm>

gui::VLayoutWidget::VLayoutWidget(GUI* gui, nlohmann::json j) : ContainerWidget(gui, j)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

bool gui::VLayoutWidget::init(nlohmann::json j, bool ignoreType)
{
	if (ContainerWidget::init(j, true))
	{
		if (checkWidgetType<VLayoutWidget>(ignoreType))
		{
		}
	}
	return true;
}

void gui::VLayoutWidget::revalidate()
{
	ContainerWidget::revalidate();

	std::vector<Widget*> visibleChildren = getVisibleChildren();

	if (visibleChildren.size() > 0)
	{
		float midx = w / 2.0f;
		float midy = h / 2.0f;
		float size = 0.0f;
		float maxx = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->h + widget->weight * (widget->proportional ? h : 1);
			maxx = std::max(maxx, (float)widget->w);
		}

		switch (alignment)
		{
		case ALIGNMENT::ALIGN_LIST:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_CENTERED:
		{
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = midx - visibleChildren[i]->w / 2.0f;
				visibleChildren[i]->y = visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1) - (visibleChildren[i]->centered * visibleChildren[i]->h / 2.0f);
			}
			break;
		}
		case ALIGNMENT::ALIGN_TOP:
		{
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = midx - visibleChildren[i]->w / 2.0f;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_BOTTOM:
		{
			float start = h - size - padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = midx - visibleChildren[i]->w / 2.0f;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_LEFT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = spacing;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RIGHT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = w - visibleChildren[i]->w;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_STACKED:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = midx - visibleChildren[i]->w / 2.0f;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_START:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = start;
				visibleChildren[i]->x = 0.0f;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_END:
		{
			float leftOver = h - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = w - visibleChildren[i]->w;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RSTART:
		{
			float leftOver = h - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = start;
				visibleChildren[i]->x = 0.0f;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_REND:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = w - visibleChildren[i]->w;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_SPACED:
		{
			float leftOver = h - size - padding * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = midx - visibleChildren[i]->w / 2.0f;
				visibleChildren[i]->y = start;
				start += visibleChildren[i]->h + d + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? h : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->h / 2.0f;
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