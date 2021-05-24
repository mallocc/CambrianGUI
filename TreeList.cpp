#include "TreeList.h"
#include <GUI.h>
#include <Label.h>
#include <VLayout.h>
#include <HLayout.h>

gui::TreeNode::TreeNode(TreeNode* parent, std::string id, std::string text)
	: parent(parent),
	id(id),
	text(text)
{
}

void gui::TreeNode::addChild(TreeNode* child)
{
	children.push_back(child);
}

bool gui::TreeNode::removeNode(std::string id)
{
	for (std::vector<TreeNode*>::iterator itor = children.begin();
		itor != children.end(); ++itor)
	{
		TreeNode* node = *itor;
		if (node->id == id)
		{
			children.erase(itor);
			return true;
		}
	}
	return false;
}

gui::TreeNode* gui::TreeNode::findChild(std::string id)
{
	for (TreeNode* node : children)
	{
		if (node->id == id)
		{
			return node;
		}
	}
	return nullptr;
}

nlohmann::json gui::TreeNode::getJson(nlohmann::json nodeTemplate)
{
	nlohmann::json k;
	k["widget"] = "vlayout";
	k["align"] = "list";
	k["size"] = "expand";
	k["x"] = 10;

	nlohmann::json l = nodeTemplate;
	l["text"] = id + "Node";
	l["id"] = id + "Node";
	k["children"].push_back(l);

	if (!children.empty())
	{
		for (TreeNode* child : children)
		{
			k["children"].push_back(child->getJson(nodeTemplate));
		}
	}

	return k;
}


gui::TreeList::TreeList(GUI* gui)
	: VLayout(gui), root(root)
{
	onExpand = [](bool) {};
	onItemClicked = [](Widget*) {};
}

bool gui::TreeList::init(nlohmann::json j, bool ignoreType)
{
	bool success = false;
	if (VLayout::init(j, true))
	{
		if (checkWidgetType<TreeList>(ignoreType))
		{
			ConfigList fields;
			{
				fields["node"] = nodeTemplate;
				fields["layout"] = layoutTemplate;
				fields["icon"] = iconTemplate;
			}
			fields.load(j);

			m_config += fields;

			success = true;
		}
	}
	return success;
}

void gui::TreeList::onExpandEvent(bool expanded)
{
	this->expanded = expanded;

	if (subTree != nullptr)
		subTree->setVisible(expanded);

	if (icon != nullptr)
		if (expanded)
			icon->check(false);
		else
			icon->uncheck(false);

	onExpand(expanded);
}

void gui::TreeList::onItemClickedEvent(Widget* clickedItem)
{
	for (auto& widget : getRadioChildren())
	{
		widget_as(TreeList, treeList, widget)
		{
			treeList->setSelected(false);
		}
	}

	widget_as(TreeList, treeList, clickedItem)
	{
		treeList->setSelected(true);
	}

	onItemClicked(clickedItem);
}

std::vector<gui::Widget*> gui::TreeList::getChildren()
{
	return getRadioChildren();
}

bool gui::TreeList::isChecked()
{
	return layout->isChecked();
}

bool gui::TreeList::isSelected()
{
	return selected;
}

void gui::TreeList::setSelected(bool selected)
{
	if (selected)
		layout->check(false);
	else
		layout->uncheck(false);
	this->selected = selected;
}

void gui::TreeList::initRootNode(TreeNode* root, Widget* radioParent)
{
	if (root == nullptr)
		return;

	this->root = root;

	clearChildren();

	nlohmann::json meta = getMeta();
	meta["node-id"] = root->id;
	setMeta(meta);

	nlohmann::json layoutJson = layoutTemplate;
	layoutJson["widget"] = "hlayout";
	create_widget_as(HLayout, newLayout, layoutJson)
	{
		layout = newLayout;

		//if (radioParent != this)
		{
			this->setRadioParent(radioParent);
			widget_as(TreeList, rootNode, radioParent)
			{
				rootNode->addRadioChild(this);
				newLayout->onRelease = [=](GUI* gui, MouseEventData mouseEventData)
				{
					rootNode->onItemClickedEvent(this);
				};
			}
		}

		nlohmann::json nodeJson = nodeTemplate;
		nodeJson["text"] = root->id;
		create_widget(infoWidget, nodeJson)
		{
			newLayout->addChild(infoWidget);
		}

		if (!root->children.empty())
		{
			create_widget(iconWidget, iconTemplate)
			{
				icon = iconWidget;
				iconWidget->onClick = [=](GUI* gui, MouseEventData mouseEventData)
				{
					this->onExpandEvent(!this->expanded);
				};
				newLayout->addChild(iconWidget);
			}

			newLayout->onDoubleClick = [=](GUI* gui, MouseEventData mouseEventData)
			{
				this->onExpandEvent(!this->expanded);
			};
		}

		// Add the clickable node first
		addChild(newLayout);

		// And then the sub tree if there exists children nodes
		if (!root->children.empty())
		{
			nlohmann::json j;
			j["widget"] = "list";
			j["size"] = "expand";
			j["x"] = 10;
			create_widget_as(VLayout, layout, j)
			{
				subTree = layout;
				nlohmann::json k;
				k["widget"] = "treelist";
				k["align"] = "list";
				k["size"] = "expand";
				k["node"] = nodeTemplate;
				k["layout"] = layoutTemplate;
				k["icon"] = iconTemplate;
				for (auto& child : root->children)
				{
					create_widget_as(TreeList, treelist, k)
					{
						treelist->initRootNode(child, radioParent);
						layout->addChild(treelist);
					}
				}
				addChild(layout);
			}
		}

		this->onExpandEvent(layout->isChecked());
	}

}


