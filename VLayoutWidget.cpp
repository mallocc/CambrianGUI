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
		float midx = m_w / 2.0f;
		float midy = m_h / 2.0f;
		float size = 0.0f;
		float maxx = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->m_h + widget->m_weight * (widget->m_proportional ? m_h : 1);
			maxx = std::max(maxx, (float)widget->m_w);
		}

		switch (alignment)
		{
		case ALIGNMENT::ALIGN_LIST:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_CENTERED:
		{
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = midx - visibleChildren[i]->m_w / 2.0f;
				visibleChildren[i]->m_y = visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1) - (visibleChildren[i]->m_centered * visibleChildren[i]->m_h / 2.0f);
			}
			break;
		}
		case ALIGNMENT::ALIGN_TOP:
		{
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = midx - visibleChildren[i]->m_w / 2.0f;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_BOTTOM:
		{
			float start = m_h - size - padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = midx - visibleChildren[i]->m_w / 2.0f;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_LEFT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = spacing;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RIGHT:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = m_w - visibleChildren[i]->m_w;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_STACKED:
		{
			float start = midy - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = midx - visibleChildren[i]->m_w / 2.0f;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_START:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = start;
				visibleChildren[i]->m_x = 0.0f;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_END:
		{
			float leftOver = m_h - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = m_w - visibleChildren[i]->m_w;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RSTART:
		{
			float leftOver = m_h - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = start;
				visibleChildren[i]->m_x = 0.0f;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_REND:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = m_w - visibleChildren[i]->m_w;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_SPACED:
		{
			float leftOver = m_h - size - padding * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = midx - visibleChildren[i]->m_w / 2.0f;
				visibleChildren[i]->m_y = start;
				start += visibleChildren[i]->m_h + d + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_h : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_h / 2.0f;
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