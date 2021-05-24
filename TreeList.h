#pragma once

#include <VLayout.h>

namespace gui
{
	struct TreeNode
	{
		TreeNode(TreeNode* parent, std::string id, std::string text);

		void addChild(TreeNode* child);

		bool removeNode(std::string id);

		TreeNode* findChild(std::string id);

		nlohmann::json getJson(nlohmann::json nodeTemplate);

		std::string id;
		std::string text;
		TreeNode* parent;
		std::vector<TreeNode*> children;
	};

	class TreeList : public VLayout, public WidgetType<TreeList>
	{
		DEFINE_WIDGET_TYPE("treelist");
	public:

		TreeList(GUI* gui);

		virtual bool init(nlohmann::json j, bool ignoreType = false) override;

		virtual void onExpandEvent(bool expanded);
		virtual void onItemClickedEvent(Widget* clickedItem);

		std::function<void(bool)> onExpand;
		std::function<void(Widget*)> onItemClicked;
			
		void initRootNode(TreeNode* root, Widget* radioParent);

		virtual std::vector<Widget*> getChildren();

		virtual bool isChecked() override;

		bool isSelected();
		void setSelected(bool selected = true);

		Widget* layout = nullptr;
		Widget* icon = nullptr;
		Widget* subTree = nullptr;

		bool expanded = false;
		bool selected = false;

		TreeNode* root = nullptr;

		nlohmann::json nodeTemplate;
		nlohmann::json layoutTemplate;
		nlohmann::json iconTemplate;

	};
};