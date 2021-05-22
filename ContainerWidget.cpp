#include "ContainerWidget.h"
#include "GUI.h"
#include "SwitchWidget.h"
#include <GL/glew.h>

using gui::Container;
using gui::Widget;

Container::Container(GUI* gui, nlohmann::json j) : Widget(gui)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

int32_t gui::Container::getChildIndex(Widget* child)
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

void gui::Container::clearChildren()
{
	for (Widget* c : children)
	{
		if (c != nullptr)
		{
			Container* container = dynamic_cast<Container*>(c);
			if (container != nullptr)
				container->clearChildren();
			getGUI()->getWidgetManager()->removeWidget(c->getId());
		}
	}
	children.clear();
}

std::vector<Widget*> gui::Container::getVisibleChildren()
{
	std::vector<Widget*> visibleChildren;
	for (Widget* c : children)
		if (c->isVisible())
			visibleChildren.push_back(c);
	return visibleChildren;
}

void gui::Container::addChild(Widget* widget)
{
	if (widget != nullptr)
	{
		widget->setParent(this);
		children.push_back(widget);
	}
}

void gui::Container::addRadioChild(Widget* component)
{
	if (component != nullptr)
	{
		component->setRadioParent(this);
		radioChildren.push_back(component);
	}
}

Widget* gui::Container::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();

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
					if (childEnvoke && !lastLocalWidgetHandled->isExclusiveEnvoke())
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

bool gui::Container::onLeaveEvent(MouseEventData mouseEventData, bool process)
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

bool gui::Container::init(nlohmann::json j, bool ignoreType)
{
	std::string tmp;

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
				fields["child-envoke"] = childEnvoke;
				fields["padding"] = padding;
				fields["align"] = {
					"none",
							[&](std::string value)
					{
						for (int i = 0; i < ALIGNMENT::ALIGN_NUMBER; ++i)
							if (ALIGN_STRINGS[i] == value)
								alignment = (ALIGNMENT)i;
						std::cout << "DEBUG: id=" << getId() << " value=" << value << " alignment=" << alignment << std::endl;
				},
							[&](std::string fieldName) { return nlohmann::json({{fieldName, ALIGN_STRINGS[alignment]}}); }
				};
				fields["size"] = {
					"inherit",
							[&](std::string value)
					{
						for (int i = 0; i < SIZING::SIZE_NUMBER; ++i)
							if (SIZE_STRINGS[i] == value)
								sizing = (SIZING)i;
						std::cout << "DEBUG: id=" << getId() <<" value=" << value << " sizing=" << sizing << std::endl;
				},
							[&](std::string fieldName) { return nlohmann::json({{fieldName, SIZE_STRINGS[sizing]}}); }
				};
				fields["spacing"] = spacing;
			}
			fields.load(j);

			m_config += fields;
		}
	}

	return true;
}

nlohmann::json gui::Container::toJson()
{
	nlohmann::json j = m_config.toJson();

	for (Widget* widget : children)
	{
		j["children"].push_back(widget->toJson());
	}

	return j;
}

void gui::Container::revalidate()
{
	Widget::revalidate();

	if (sizing == SIZING::SIZE_INHERIT)
	{
		if (W() == 0)
		{
			if (getParent() != nullptr)
			{
				setW(getParent()->W(), FORCE);
			}
			else
			{
				setW(getGUI()->w, FORCE);
			}
		}
		if (H() == 0)
		{
			if (getParent() != nullptr)
			{
				setH(getParent()->H(), FORCE);
			}
			else
			{
				setH(getGUI()->h, FORCE);
			}
		}
	}

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (Widget* widget : visibleChildren)
	{
		widget->revalidate();
	}

	expand();
}

void gui::Container::expand()
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	switch (sizing)
	{
	case SIZE_EXPAND:
	{
		float maxx = 0.0f;
		float maxy = 0.0f;
		float minx = getGUI()->w;
		float miny = getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				minx = std::min(minx, (float)widget->X());
				miny = std::min(miny, (float)widget->Y());
			}
		}
		setW(maxx - minx, FORCE);
		setH(maxy - miny, FORCE);
		break;
	}
	case SIZE_EXPAND_WIDTH:
	{
		float maxx = 0.0f;
		float minx = getGUI()->w;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxx = std::max(maxx, (float)widget->X() + (float)widget->W());
				minx = std::min(minx, (float)widget->X());
			}
		}

		setW(maxx - minx, FORCE);
		if (getParent() != nullptr)
		{
			setH(getParent()->H(), FORCE);
		}
		else
		{
			setH(getGUI()->h, FORCE);
		}
		break;
	}
	case SIZE_EXPAND_HEIGHT:
	{
		float maxy = 0.0f;
		float miny = getGUI()->h;
		for (Widget* widget : visibleChildren)
		{
			if (!widget->isLayoutOmit())
			{
				maxy = std::max(maxy, (float)widget->Y() + (float)widget->H());
				miny = std::min(miny, (float)widget->Y());
			}
		}

		if (getParent() != nullptr)
		{
			setW(getParent()->W(), FORCE);
		}
		else
		{
			setW(getGUI()->w, FORCE);
		}
		setH(maxy - miny, FORCE);
		break;
	}
	}
}

void gui::Container::onIntent(nlohmann::json intent)
{

}


std::vector<gui::Widget*> gui::Container::getCheckedChildren()
{
	std::vector<Widget*> checkedChildren;
	for (auto& child : children)
		if (child->isChecked())
			checkedChildren.push_back(child);
	return checkedChildren;
}
