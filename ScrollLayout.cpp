#include "ScrollLayout.h"
#include "WidgetManager.h"
#include "Slider.h"
#include "GUI.h"

gui::ScrollLayout::ScrollLayout(GUI* gui) : Layout(gui)
{
}

void gui::ScrollLayout::revalidate()
{
	Layout::revalidate();

	if (m_vscrollbar != nullptr)
	{
		m_vscrollbar->setX(W() - m_vscrollbar->W());
		m_vscrollbar->setH(H());
		if (m_vscrollbar->getSlider() != nullptr)
		{
			//m_vscrollbar->getSlider()->setH(std::max(10.0f, H() / m_contentHolder->H() * H()));
		}
		if (m_contentHolder != nullptr)
		{
			if (m_contentHolder->H() > H())
			{
				m_vscrollbar->show();
				m_contentHolder->setY(-m_vscrollbar->getValToRatio() * (m_contentHolder->H() - H() + (m_hscrollbar != nullptr && m_hscrollbar->isVisible() ? m_hscrollbar->H() : 0.0f)));
			}
			else
			{
				m_vscrollbar->hide();
			}
		}
	}
	if (m_hscrollbar != nullptr)
	{
		m_hscrollbar->setY(H() - m_hscrollbar->H());
		m_hscrollbar->setW(W() - (m_vscrollbar != nullptr && m_vscrollbar->isVisible() ? m_vscrollbar->W() : 0.0f));
		if (m_hscrollbar->getSlider() != nullptr)
		{
			//m_hscrollbar->getSlider()->setW(std::max(10.0f, W() / m_contentHolder->W() * W()));
		}
		if (m_contentHolder != nullptr)
		{
			if (m_contentHolder->W() > W())
			{
				m_hscrollbar->show();
				m_contentHolder->setX(-m_hscrollbar->getValToRatio() * (m_contentHolder->W() - W() + (m_vscrollbar != nullptr && m_vscrollbar->isVisible() ? m_vscrollbar->W() : 0.0f)));
			}
			else
			{
				m_hscrollbar->hide();
			}
		}
	}
}

bool gui::ScrollLayout::init(const nlohmann::json& j, bool ignoreType)
{
	if (Layout::init(j, true))
	{
		if (checkWidgetType<ScrollLayout>(ignoreType))
		{
			ConfigList fields;
			{
				fields["vscrollbar"] = m_vscrollbarJson;
				fields["hscrollbar"] = m_hscrollbarJson;
				fields["content"] = m_contentJson;
			}
			fields.load(j);
			m_config += fields;

			nlohmann::json contentHolderJson;
			contentHolderJson["widget"] = "layout";
			contentHolderJson["expand"] = "prefered";
			contentHolderJson["align"] = "start";
			contentHolderJson["children"] = m_contentJson;

			create_widget_as(Slider, vscrollbar, m_vscrollbarJson)
			{
				m_vscrollbar = vscrollbar;
				addChild(vscrollbar);
			}

			create_widget_as(Slider, hscrollbar, m_hscrollbarJson)
			{
				m_hscrollbar = hscrollbar;
				addChild(hscrollbar);
			}

			create_widget_as(Layout, contentHolder, contentHolderJson)
			{
				m_contentHolder = contentHolder;
				addChild(contentHolder);
			}


		}
	}
	return true;
}
