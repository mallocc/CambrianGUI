#pragma once

#include <chrono>
#include <AssetPacker.h>
#include <SplitTimer.h>
#include "WidgetManager.h"
#include "ShaderManager.h"
#include "FontManager.h"

namespace gui
{
	extern const std::map<std::string, LPSTR> cursors;

	typedef std::function<void(GUI*)> TriggerCallback;

	class GUI
	{
	public:
		GUI(int32_t w, int32_t h);

		GUI* getGUI();

		void resize(float w, float h);
		virtual void draw();
		virtual void onMouseEvent(MouseEventData mouseEventData);
		virtual void onMouseLeave();
		virtual void onKeyEvent(KeyEventData keyEventData);
		virtual bool keyDown(int virtualKey, KeyEventData* keys);
		virtual bool keyUp(int virtualKey, KeyEventData* keys);
		virtual void init(std::string configOverridePath = "", bool firstLoad = 1);
		virtual void loadDimensions(std::string configOverridePath);
		virtual void fireTriggers(nlohmann::json triggerJson);
		virtual void free();
		TextureManager* getTextureManager();
		FontManager* getFontManager();
		ShaderManager* getShaderManger();
		WidgetManager* getWidgetManager();
		Configuration* getGUIConfig();

		virtual void showCursor();
		virtual void hideCursor();
		virtual void setCursor(std::string cursor);

		virtual void openDropdownIntent(Widget* parent, nlohmann::json j, intentcallback_t selectionCallback);
		virtual void closeDropdownIntent();

		virtual void openRightClickIntent(nlohmann::json j, intentcallback_t selectionCallback);

		virtual void showFloatingLabel(int32_t x, int32_t y, std::string text, uint64_t showTime = 0UL);
		virtual void hideFloatingLabel();

		virtual void showHintLabel(std::string text, uint64_t showTime = 0UL);
		virtual void hideHintLabel();

		virtual void registerTriggerCallback(std::string triggerName, TriggerCallback function);

		AssetPacker::fileImageMap_t filesys;

		nlohmann::json toJson();

		SplitTimer::Timer *getTimer();


		std::filesystem::path configureOverride;
		bool configInvalidated;

		Texture* defaultNormalMap;
		GLuint cubemap;
		int32_t w = 0, h = 0;
		bool editMode = false;
		bool firstLoad = true;
		bool displayCredits = false;
		bool enableDrawing = true;

		float frameCount = 0;
		uint64_t currentShowTimeHintLabel = 0UL;
		uint64_t currentShowTimeFloatingLabel = 0UL;


		HCURSOR hCursor;

		Widget* lastHandledWidget = nullptr;
	private:
		Configuration* config = nullptr;
		WidgetManager* widgetManager = nullptr;

		ShaderManager* shaderManager = nullptr;
		TextureManager* texMan = nullptr;
		std::chrono::time_point<std::chrono::steady_clock> oldClickTime;
		Widget* focusedWidget = nullptr;
		MouseEventData oldMouseEventData;
		MouseEventData focusedMouseEventData;
		KeyEventData oldKeyEventData;
		FontManager* fontMan = nullptr;
		int cursorVisibility = 0;
		float lastEditTime;
		SplitTimer::Timer timer;

		std::map<std::string, TriggerCallback> triggerCallbacks;

	};

}