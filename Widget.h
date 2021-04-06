#pragma once

#include <chrono>
#include <functional>
#include <nlohmann/json.hpp>

#include "Color.h"
#include "EventData.h"
#include "TextureManager.h"
#include "ShaderProperties.h"

namespace gui
{
	class GUI;

	typedef std::function<void(gui::GUI*, MouseEventData)> callback_t;
	typedef std::function<void(gui::GUI*, KeyEventData)> keycallback_t;


	struct Widget
	{
		GUI* gui;
		std::string type = "undefined";
		std::string classname = "widget";
		float x = 0, y = 0;
		float xOffset, yOffset = 0;
		float targetX = 0, targetY = 0;
		float w = 0, h = 0;
		float targetW = 0, targetH = 0;
		float weight = 0.0f;
		bool  proportional = false;
		bool  centered = false;
		float rotation = 0.0f;
		float targetRotation = 0.0f;
		Texture* background = nullptr;
		Texture* backgroundTransition = background;
		float backgroundTransitionValue = 0.0f;
		float transitionSpeed = 1.0f;
		bool backgroundTiled = 0;
		std::string text = "---";
		Widget* parent = nullptr;
		MouseEventData oldMouseEventData;
		MouseEventData initialMouseEventData;
		KeyEventData oldKeyEventData;
		std::chrono::time_point<std::chrono::steady_clock> oldClickTime;
		std::chrono::time_point<std::chrono::steady_clock> oldHoverTime;
		uint64_t hintShowTime = 100UL;

		std::string widgetId = "widget";

		std::string blendMode = "normal";

		std::string shellExecute = "";

		std::string hint = "";

		float opacity = 1.0f;

		callback_t onClick;
		callback_t onRightClick;
		callback_t onRelease;
		callback_t onDoubleClick;
		callback_t onOver;
		callback_t onDown;
		callback_t onUp;
		callback_t onLeave;
		callback_t onDrag;
		callback_t onMiddleClick;
		callback_t onToggledOn;
		callback_t onToggledOff;

		nlohmann::json config;
		nlohmann::json onOverJson;
		nlohmann::json onLeaveJson;
		nlohmann::json onReleaseJson;
		nlohmann::json onToggledOnJson;
		nlohmann::json onClickJson;
		nlohmann::json onToggledOffJson;
		nlohmann::json onClickExternalJson;
		nlohmann::json onReleaseExternalJson;
		nlohmann::json onToggledOnExternalJson;
		nlohmann::json onToggledOffExternalJson;



		Color color = { 1, 1, 1 };
		Color colorStart = { 1, 1, 1 };
		Color colorEnd = { 1, 1, 1 };
		Color targetColor = color;

		float sensitivity = 300.0f;
		float sensitivityShift = sensitivity * 10.0f;
		bool toggleOnClick = false;
		bool down = false;
		bool over = false;

		bool exclusiveEnvoke = false;

		bool visible = true;
		bool clickable = true;
		bool clickThrough = true;

		bool scaled = false;

		bool toggled = false;
		bool toggleable = false;

		bool radio = false;

		std::string cursor = "";

		ShaderProperties shaderProperties;

		std::map<std::string, ManifestTuple> manifestList;

		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual Widget* onKeyEvent(KeyEventData keyEventData);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual void revalidate();
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual nlohmann::json toJson();
		virtual void addToManifestList(nlohmann::json j, std::map<std::string, ManifestTuple> manifest);

		bool keyDown(int virtualKey, KeyEventData* keys);
		bool keyUp(int virtualKey, KeyEventData* keys);

		void getAbsolutePosition(float& xPos, float& yPos);

		Widget(GUI* gui);

		virtual bool onClickEvent(MouseEventData mouseEventData, bool process);
		virtual bool onRightClickEvent(MouseEventData mouseEventData, bool process);
		virtual bool onDoubleClickEvent(MouseEventData mouseEventData, bool process);
		virtual bool onMoveEvent(MouseEventData mouseEventData, bool process);
		virtual bool onDragEvent(MouseEventData mouseEventData, bool process);
		virtual bool onReleaseEvent(MouseEventData mouseEventData, bool process);
		virtual bool onOverEvent(MouseEventData mouseEventData, bool process);
		virtual bool onLeaveEvent(MouseEventData mouseEventData, bool process);
		virtual bool onDownEvent(MouseEventData mouseEventData, bool process);
		virtual bool onUpEvent(MouseEventData mouseEventData, bool process);
		virtual bool onMiddleClickEvent(MouseEventData mouseEventData, bool process);

		virtual void onIntent(std::string intentChoice);

		void getContextPosition(float& tx, float& ty);

		void toggleOn(bool updatedRadio = false, bool force = false);
		void toggleOff(bool updatedRadio = false, bool force = false);

		bool doesTypeMatch(bool bypass = false, std::string typeQuery = "");
		void setClassname(std::string newClassname);

	};
}