#include "Container.h"
#include "WidgetManager.h"
#include "GUI.h"

using gui::Container;
using gui::Widget;

gui::Container::Container(GUI* gui) : Widget(gui)
{
}

int32_t Container::getChildIndex(Widget* child)
{
	std::vector<Widget*> children = getVisibleChildren();
	int32_t index = -1;
	for (auto c : children)
	{
		++index;
		if (c == child)
		{
			break;
		}
	}
	return index;
}

void Container::clearChildren()
{
	for (Widget* c : m_children)
	{
		if (c != nullptr)
		{
			Container* container = dynamic_cast<Container*>(c);
			if (container != nullptr)
				container->clearChildren();
			getGUI()->getWidgetManager()->removeWidget(c->getId());
		}
	}
	m_children.clear();
}

std::vector<Widget*>& Container::getVisibleChildren()
{
	std::vector<Widget*> visibleChildren;
	for (Widget* c : m_children)
		if (c->isVisible())
			visibleChildren.push_back(c);
	return visibleChildren;
}

void Container::addChild(Widget* widget)
{
	if (widget != nullptr)
	{
		widget->setParent(this);
		m_children.push_back(widget);
	}
}

void Container::addRadioChild(Widget* component)
{
	if (component != nullptr)
	{
		component->setRadioParent(this);
		m_radioChildren.push_back(component);
	}
}

Widget* Container::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();

	if (process)
	{
		Widget* oldLastLocalWidgetHandled = m_lastLocalWidgetHandled;
		m_lastLocalWidgetHandled = nullptr;
		// Handle children widgets
		if (oldLastLocalWidgetHandled != this)
		{
			for (Widget* widget : visibleChildren)
			{
				Widget* handledWidget = widget->onMouseEvent(mouseEventData, process, focus);
				if (m_lastLocalWidgetHandled == nullptr)
				{
					m_lastLocalWidgetHandled = handledWidget;
					if (m_lastLocalWidgetHandled != nullptr)
					{
						if (m_childEnvoke && !m_lastLocalWidgetHandled->isExclusiveEnvoke())
							break;
						return m_lastLocalWidgetHandled;
					}
				}
			}
		}
		if (m_lastLocalWidgetHandled == nullptr)
		{
			m_lastLocalWidgetHandled = Widget::onMouseEvent(mouseEventData, process, focus);
		}
		else
		{
			Widget::onMouseEvent(mouseEventData, process, true);
		}
		return m_lastLocalWidgetHandled;
	}
	else
	{
		Widget* lastLocalWidgetHandled = m_lastLocalWidgetHandled;
		Widget* oldLastLocalWidgetHandled = lastLocalWidgetHandled;
		lastLocalWidgetHandled = nullptr;
		// Handle children widgets
		if (oldLastLocalWidgetHandled != this)
		{
			for (Widget* widget : visibleChildren)
			{
				Widget* handledWidget = widget->onMouseEvent(mouseEventData, process, focus);
				if (lastLocalWidgetHandled == nullptr)
				{
					lastLocalWidgetHandled = handledWidget;
					if (lastLocalWidgetHandled != nullptr)
					{
						if (m_childEnvoke && !lastLocalWidgetHandled->isExclusiveEnvoke())
							break;
						return lastLocalWidgetHandled;
					}
				}
			}
		}
		if (lastLocalWidgetHandled == nullptr)
		{
			lastLocalWidgetHandled = Widget::onMouseEvent(mouseEventData, process, focus);
		}
		else
		{
			Widget::onMouseEvent(mouseEventData, process, true);
		}
		return lastLocalWidgetHandled;
	}
}

bool Container::onLeaveEvent(MouseEventData mouseEventData, bool process)
{
	Widget::onLeaveEvent(mouseEventData, process);
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (Widget* widget : visibleChildren)
	{
		widget->onLeaveEvent(mouseEventData, process);
	}
	return true;
}

Widget* Container::onKeyEvent(KeyEventData keyEventData)
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	Widget* widgetHandled = nullptr;
	// Handle children widgets
	for (Widget* widget : visibleChildren)
	{
		widgetHandled = widget->onKeyEvent(keyEventData);
		if (widgetHandled != nullptr)
		{
			return widgetHandled;
		}
	}
	if (widgetHandled == nullptr)
	{
		widgetHandled = Widget::onKeyEvent(keyEventData);
	}
	return widgetHandled;
}

void Container::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, false);
	tx += X(); ty += Y();

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (int i = visibleChildren.size() - 1; i >= 0; --i)
	{
		visibleChildren[i]->draw(tx, ty, editMode);
	}

	bool widgetHandledDown = false;
	for (Widget* widget : visibleChildren)
		if (widget->isDown())
			widgetHandledDown = true;

	if (editMode)// && gui->editedWidget == this)
	{
		glColor3f(1, 0, 0);
		glPushMatrix();
		glTranslatef(tx, ty, 0);
		glScalef(W(), H(), 1);
		glBegin(GL_LINE_LOOP);
		{
			glVertex2f(0, 0);
			glVertex2f(1, 0);
			glVertex2f(1, 1);
			glVertex2f(0, 1);
		}
		glEnd();
		glPopMatrix();
	}
}

bool Container::init(nlohmann::json j, bool ignoreType)
{
	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<Container>(ignoreType))
		{
			if (checkJSON(j, "children"))
			{
				nlohmann::json jc = j.at("children");
				for (auto& i : jc)
				{
					create_widget(widget, i)
					{
						addChild(widget);
					}
				}
			}

			ConfigList fields;
			{
				fields["child-envoke"] = m_childEnvoke;
			}
			fields.load(j);

			m_config += fields;
		}
	}

	return true;
}

nlohmann::json Container::toJson()
{
	nlohmann::json j = m_config.toJson();

	for (Widget* widget : m_children)
	{
		j["children"].push_back(widget->toJson());
	}

	return j;
}

void Container::revalidate()
{
	Widget::revalidate();

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (Widget* widget : visibleChildren)
	{
		widget->revalidate();
	}
}

void Container::onIntent(nlohmann::json intent)
{

}


std::vector<gui::Widget*>& Container::getCheckedChildren()
{
	std::vector<Widget*> checkedChildren;
	for (auto& child : m_children)
		if (child->isChecked())
			checkedChildren.push_back(child);
	return checkedChildren;
}

void Container::setChildEnvoke(bool childEnvoke)
{
	this->m_childEnvoke = childEnvoke;
}

bool Container::isChildEnvoke()
{
	return m_childEnvoke;
}

std::vector<Widget*>& Container::getRadioChildren()
{
	return m_radioChildren;
}

std::vector<Widget*>& Container::getChildren()
{
	return m_children;
}
