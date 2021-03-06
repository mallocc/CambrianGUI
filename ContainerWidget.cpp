#include "ContainerWidget.h"
#include "GUI.h"
#include "SwitchWidget.h"
#include <GL/glew.h>

using gui::ContainerWidget;
using gui::Widget;

ContainerWidget::ContainerWidget(GUI* gui, nlohmann::json j) : Widget(gui)
{
	if (!init(j))
	{
		std::cout << "Failed to init widget: " << getWidgetType() << std::endl;
	}
}

int32_t gui::ContainerWidget::getChildIndex(Widget* child)
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

void gui::ContainerWidget::clearChildren()
{
	for (Widget* c : children)
	{
		if (c != nullptr)
		{
			ContainerWidget* container = dynamic_cast<ContainerWidget*>(c);
			if (container != nullptr)
				container->clearChildren();
			gui->getWidgetManager()->removeWidget(c->widgetId);
		}
	}
	children.clear();
}

std::vector<Widget*> gui::ContainerWidget::getVisibleChildren()
{
	std::vector<Widget*> visibleChildren;
	for (Widget* c : children)
		if (c->visible)
			visibleChildren.push_back(c);
	return visibleChildren;
}

void gui::ContainerWidget::addChild(Widget* widget)
{
	if (widget != nullptr)
	{
		widget->parent = this;
		children.push_back(widget);
	}
}

void gui::ContainerWidget::addRadioChild(Widget* component)
{
	if (component != nullptr)
	{
		component->radioParent = this;
		radioChildren.push_back(component);
	}
}

Widget* gui::ContainerWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
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
					if (childEnvoke && !lastLocalWidgetHandled->exclusiveEnvoke)
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

bool gui::ContainerWidget::onLeaveEvent(MouseEventData mouseEventData, bool process)
{
	Widget::onLeaveEvent(mouseEventData, process);
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (Widget* widget : visibleChildren)
	{
		widget->onLeaveEvent(mouseEventData, process);
	}
	return true;
}

Widget* ContainerWidget::onKeyEvent(KeyEventData keyEventData)
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

void ContainerWidget::draw(float tx, float ty, bool editMode)
{
	Widget::draw(tx, ty, false);
	tx += x; ty += y;

	std::vector<Widget*> visibleChildren = getVisibleChildren();
	for (int i = visibleChildren.size() - 1; i >= 0; --i)
	{
		visibleChildren[i]->draw(tx, ty, editMode);
	}

	bool widgetHandledDown = false;
	for (Widget* widget : visibleChildren)
		if (widget->down)
			widgetHandledDown = true;

	if (editMode)// && gui->editedWidget == this)
	{
		glColor3f(1, 0, 0);
		glPushMatrix();
		glTranslatef(tx, ty, 0);
		glScalef(w, h, 1);
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

bool gui::ContainerWidget::init(nlohmann::json j, bool ignoreType)
{
	std::string tmp;

	if (Widget::init(j, ignoreType))
	{
		if (checkWidgetType<ContainerWidget>(ignoreType))
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
				fields["background-tiled"] = backgroundTiled;
				fields["align"] = {
					"NONE",
							[&](std::string value) { for (int i = 0; i < ALIGNMENT::ALIGN_NUMBER; ++i)	if (ALIGN_STRINGS[i] == value) alignment = (ALIGNMENT)i; },
							[&](std::string fieldName) { return nlohmann::json({{fieldName, ALIGN_STRINGS[alignment]}}); }
				};
				fields["size"] = {
					"INHERIT",
							[&](std::string value) { for (int i = 0; i < SIZING::SIZE_NUMBER; ++i) if (SIZE_STRINGS[i] == value) sizing = (SIZING)i; },
							[&](std::string fieldName) { return nlohmann::json({{fieldName, SIZE_STRINGS[sizing]}}); }
				};
				fields["spacing"] = spacing;
				fields["radio"] = radio;
			}
			fields.load(j);

			config += fields;
		}
	}

	return true;
}

nlohmann::json gui::ContainerWidget::toJson()
{
	nlohmann::json j = config.toJson();

	for (Widget* widget : children)
	{
		j["children"].push_back(widget->toJson());
	}

	return j;
}

void gui::ContainerWidget::revalidate()
{
	Widget::revalidate();

	if (sizing == SIZING::SIZE_INHERIT)
	{
		if (w == 0)
		{
			if (parent != nullptr)
			{
				w = parent->w;
			}
			else
			{
				w = gui->w;
			}
		}
		if (h == 0)
		{
			if (parent != nullptr)
			{
				h = parent->h;
			}
			else
			{
				h = gui->h;
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

void gui::ContainerWidget::expand()
{
	std::vector<Widget*> visibleChildren = getVisibleChildren();
	switch (sizing)
	{
	case SIZE_EXPAND:
	{
		float maxx = 0.0f;
		float maxy = 0.0f;
		float minx = gui->w;
		float miny = gui->h;
		for (Widget* widget : visibleChildren)
		{
			maxx = std::max(maxx, (float)widget->x + (float)widget->w);
			maxy = std::max(maxy, (float)widget->y + (float)widget->h);
			minx = std::min(minx, (float)widget->x);
			miny = std::min(miny, (float)widget->y);
		}
		w = maxx - minx;
		h = maxy - miny;
		break;
	}
	case SIZE_EXPAND_WIDTH:
	{
		float maxx = 0.0f;
		float minx = gui->w;
		for (Widget* widget : visibleChildren)
		{
			maxx = std::max(maxx, (float)widget->x + (float)widget->w);
			minx = std::min(minx, (float)widget->x);
		}

		w = maxx - minx;
		if (parent != nullptr)
		{
			h = parent->h;
		}
		else
		{
			h = gui->h;
		}
		break;
	}
	case SIZE_EXPAND_HEIGHT:
	{
		float maxy = 0.0f;
		float miny = gui->h;
		for (Widget* widget : visibleChildren)
		{
			maxy = std::max(maxy, (float)widget->y + (float)widget->h);
			miny = std::min(miny, (float)widget->y);
		}

		if (parent != nullptr)
		{
			w = parent->w;
		}
		else
		{
			w = gui->w;
		}
		h = maxy - miny;
		break;
	}
	}
}

void gui::ContainerWidget::onIntent(nlohmann::json intent)
{

}


std::vector<gui::Widget*> gui::ContainerWidget::getCheckedChildren()
{
	std::vector<Widget*> checkedChildren;
	for (auto& child : children)
		if (child->isChecked())
			checkedChildren.push_back(child);
	return checkedChildren;
}
