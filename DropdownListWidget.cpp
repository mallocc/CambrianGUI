#include "DropdownListWidget.h"
#include "GUI.h"


bool gui::DropdownListWidget::init(nlohmann::json j, bool ignoreType)
{
	if (VLayoutWidget::init(j, true))
	{
		if (doesTypeMatch(ignoreType))
		{
			visible = false;

			addToManifestList(j,
				{
					{
						"labelwidget",
						{"",
						[&](std::string value) { labelTemplate = nlohmann::json::parse(value); },
						[&](std::string fieldName) { return nlohmann::json({{ fieldName, labelTemplate.dump(4) }}); }}
					},
				}
			);	
		}
	}

	return true;
}

bool gui::DropdownListWidget::initList(nlohmann::json j)
{
	clearChildren();

	for (auto& d : j)
	{
		labelTemplate["text"] = d[0];
		labelTemplate["w"] = w;
		LabelWidget* label = dynamic_cast<LabelWidget*>(gui->getWidgetManager()->createWidget(labelTemplate));
		if (label != nullptr)
		{
			label->onClick = [=](GUI* gui, MouseEventData mouseEventData)
			{
				this->onIntent(d.dump(4));
			};
		}
		addChild(label);
	}

	return true;
}

void gui::DropdownListWidget::draw(float tx, float ty, bool editMode)
{
	if (parent != nullptr)
	{
		parent->getAbsolutePosition(tx, ty);
		ty += parent->h;
	}
	VLayoutWidget::draw(tx, ty, editMode);
}

gui::Widget* gui::DropdownListWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	if (parent != nullptr)
		mouseEventData.y -= parent->h;
	return VLayoutWidget::onMouseEvent(mouseEventData, process, focus);
}

void gui::DropdownListWidget::onIntent(std::string intentChoice)
{
	if (parent != nullptr)
	{
		parent->onIntent(intentChoice);
	}
	visible = false;
}

gui::DropdownListWidget::DropdownListWidget(GUI* gui, nlohmann::json j) : VLayoutWidget(gui, j)
{
	setClassname(DROPDOWNLISTWIDGET_CLASSNAME);
	init(j);
}
