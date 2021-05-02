#include "HLayoutWidget.h"
#include "GUI.h"

gui::HLayoutWidget::HLayoutWidget(GUI* gui, nlohmann::json j)
	: ContainerWidget(gui, j)
{
	setClassname(HLAYOUTWIDGET_CLASSNAME);
	init(j);
}

bool gui::HLayoutWidget::init(nlohmann::json j, bool ignoreType)
{
	if (ContainerWidget::init(j, true))
	{
		if (doesTypeMatch(ignoreType))
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
		float midx = w / 2.0f;
		float midy = h / 2.0f;
		float size = 0.0f;
		float maxy = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->w + widget->weight * (widget->proportional ? w : 1);
			maxy = std::max(maxy, (float)widget->h);
		}

		switch (alignment)
		{
		case ALIGNMENT::ALIGN_LIST:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_CENTERED:
		{
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = midx - visibleChildren[i]->h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_LEFT:
		{
			float start = 0;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = midy - visibleChildren[i]->h / 2.0f;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RIGHT:
		{
			float start = w - size;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = midy - visibleChildren[i]->h / 2.0f;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + +visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_TOP:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = 0;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_BOTTOM:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = h - visibleChildren[i]->h;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_STACKED:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = midy - visibleChildren[i]->h / 2.0f;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_START:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = start;
				visibleChildren[i]->y = 0.0f;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_END:
		{
			float leftOver = w - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = h - visibleChildren[i]->h;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RSTART:
		{
			float leftOver = w - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->x = start;
				visibleChildren[i]->y = 0.0f;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_REND:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = h - visibleChildren[i]->h;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_SPACED:
		{
			float leftOver = w - size - padding * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->y = midy - visibleChildren[i]->h / 2.0f;
				visibleChildren[i]->x = start;
				start += visibleChildren[i]->w + d + visibleChildren[i]->weight * (visibleChildren[i]->proportional ? w : 1);
				if (visibleChildren[i]->centered)
					start -= visibleChildren[i]->w / 2.0f;
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