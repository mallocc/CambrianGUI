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
		float midx = m_w / 2.0f;
		float midy = m_h / 2.0f;
		float size = 0.0f;
		float maxy = 0.0f;
		for (Widget* widget : visibleChildren)
		{
			size += widget->m_w + widget->m_weight * (widget->m_proportional ? m_w : 1);
			maxy = std::max(maxy, (float)widget->m_h);
		}

		switch (alignment)
		{
		case ALIGNMENT::ALIGN_LIST:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_CENTERED:
		{
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = midx - visibleChildren[i]->m_h / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_LEFT:
		{
			float start = 0;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = midy - visibleChildren[i]->m_h / 2.0f;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RIGHT:
		{
			float start = m_w - size;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = midy - visibleChildren[i]->m_h / 2.0f;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + +visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_TOP:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = 0;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_BOTTOM:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = m_h - visibleChildren[i]->m_h;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_STACKED:
		{
			float start = midx - size / 2.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = midy - visibleChildren[i]->m_h / 2.0f;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_START:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = start;
				visibleChildren[i]->m_y = 0.0f;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_END:
		{
			float leftOver = m_w - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = m_h - visibleChildren[i]->m_h;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_RSTART:
		{
			float leftOver = m_w - size;
			float start = leftOver;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_x = start;
				visibleChildren[i]->m_y = 0.0f;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_REND:
		{
			float start = 0.0f;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = m_h - visibleChildren[i]->m_h;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
			}
			break;
		}
		case ALIGNMENT::ALIGN_SPACED:
		{
			float leftOver = m_w - size - padding * 2.0f;
			float d = leftOver / (visibleChildren.size() - 1);
			float start = padding;
			for (int i = 0; i < visibleChildren.size(); ++i)
			{
				visibleChildren[i]->m_y = midy - visibleChildren[i]->m_h / 2.0f;
				visibleChildren[i]->m_x = start;
				start += visibleChildren[i]->m_w + d + visibleChildren[i]->m_weight * (visibleChildren[i]->m_proportional ? m_w : 1);
				if (visibleChildren[i]->m_centered)
					start -= visibleChildren[i]->m_w / 2.0f;
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