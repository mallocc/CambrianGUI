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



	template<typename T>
	struct WidgetType
	{
		static std::string getWidgetType()
		{
			return T::getWidgetType();
		}
	};


#define DEFINE_WIDGET_TYPE(WIDGET_TYPENAME_SYMBOL) \
	static std::string getWidgetType() { return WIDGET_TYPENAME_SYMBOL; }

	struct Widget : WidgetType<Widget>
	{
		DEFINE_WIDGET_TYPE("widget");

		GUI* gui;

		float x = 0, y = 0;
		float xOffset, yOffset = 0;
		float xTarget = 0, yTarget = 0;
		float w = 0, h = 0;
		float wTarget = 0, hTarget = 0;
		float weight = 0.0f;
		bool  proportional = false;
		bool  centered = false;
		float rotation = 0.0f;
		float rotationTarget = 0.0f;
		Texture* background = nullptr;
		Texture* backgroundTransition = background;
		float backgroundTransitionValue = 0.0f;
		float transitionSpeed = 1.0f;
		bool backgroundTiled = 0;
		std::string type = "undefined";
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
		callback_t onChecked;
		callback_t onUnchecked;

		nlohmann::json defaultJson;
		nlohmann::json onOverJson;
		nlohmann::json onLeaveJson;
		nlohmann::json onClickJson;
		nlohmann::json onReleaseJson;
		nlohmann::json onCheckedJson;
		nlohmann::json onUncheckedJson;
		nlohmann::json onClickExternalJson;
		nlohmann::json onReleaseExternalJson;
		nlohmann::json onCheckedExternalJson;
		nlohmann::json onUncheckedExternalJson;

		nlohmann::json data;

		Color color = { 1, 1, 1 };
		Color colorStart = { 1, 1, 1 };
		Color colorEnd = { 1, 1, 1 };
		Color targetColor = color;

		float sensitivity = 300.0f;
		float sensitivityShift = sensitivity * 10.0f;
		bool checkOnClick = false;
		bool down = false;
		bool over = false;

		bool exclusiveEnvoke = false;

		bool visible = true;
		bool clickable = true;
		bool clickThrough = true;

		bool scaled = false;

		bool checked = false;
		bool checkable = false;

		bool radio = false;
		Widget* radioParent = nullptr;
		virtual void radioUp();

		std::string cursor = "";

		ShaderProperties shaderProperties;

		ConfigManifest config;

		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual Widget* onKeyEvent(KeyEventData keyEventData);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual void revalidate();
		virtual bool init(nlohmann::json j, bool ignoreType = false);
		virtual nlohmann::json toJson();
		virtual void addToManifestList(nlohmann::json j, ConfigManifest config);

		template <typename T>
		void addConfigItem(std::string fieldName, T& reference, std::string defaultValue = "", initcallback_t initCallback = nullptr, jsoncallback_t jsonCallback = nullptr);

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

		virtual void onIntent(nlohmann::json intent = nlohmann::json());

		void getContextPosition(float& tx, float& ty);

		void check(bool updatedRadio = true, bool force = false);
		void uncheck(bool updatedRadio = true, bool force = false);
		void toggleCheck(bool updatedRadio = true, bool force = false);

		bool getColor(std::string colorName, Color& color);
		std::string getColorName(Color& color);
		Texture* requireTexture(std::string path);
		ShaderProperties getShaderProperties(std::string value);

		template<typename W>
		bool checkWidgetType(bool ignoreCheck = false)
		{
			return WidgetType<W>::getWidgetType() == type || ignoreCheck;
		}


		ManifestTuple colorConfigItem(Color& reference, std::string defaultValue = "");

		typedef Texture* TexturePtr;
		ManifestTuple textureConfigItem(TexturePtr& reference, std::string defaultValue = "");

		ManifestTuple shaderPropertiesConfigItem(ShaderProperties& reference, std::string defaultValue = "");
	};


	template <typename T>
	void gui::Widget::addConfigItem(std::string fieldName, T& reference, std::string defaultValue, initcallback_t initCallback, jsoncallback_t jsonCallback)
	{
		if (initCallback == nullptr)
		{
			if constexpr (std::is_same_v<T, nlohmann::json>)
			{
				if (defaultValue.empty())
					defaultValue = "{}";
				initCallback = [&](std::string value) {
					reference = nlohmann::json::parse(value);
				};
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				if (defaultValue.empty())
					defaultValue = "";
				initCallback = [&](std::string value) {
					reference = value;
				};
			}
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::atof(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int8_t> || std::is_same_v<T, int64_t>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::atoi(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint64_t>)
			{
				if (defaultValue.empty())
					defaultValue = "0";
				initCallback = [&](std::string value) {
					reference = std::stoul(value.c_str());
				};
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				if (defaultValue.empty())
					defaultValue = "false";
				initCallback = [&](std::string value) {
					reference = value == "true";
				};
			}
			else if constexpr (std::is_same_v<T, Color>)
			{
				if (defaultValue.empty())
					defaultValue = "#ffffff";
				initCallback = [&](std::string value) {
					getColor(value, reference);
				};
			}
			else if constexpr (std::is_same_v<T, Texture*>)
			{
				if (defaultValue.empty())
					defaultValue = "";
				initCallback = [&](std::string value) {
					reference = requireTexture(value);
				};
			}
			else if constexpr (std::is_same_v<T, ShaderProperties>)
			{
				if (defaultValue.empty())
					defaultValue = "{}";
				initCallback = [&](std::string value) {
					reference = getShaderProperties(value);
				};
			}
		}

		if (jsonCallback == nullptr)
		{
			if constexpr (std::is_same_v<T, Color>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ { value, getColorName(reference) } });
				};
			}
			else if constexpr (std::is_same_v<T, nlohmann::json>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ { value, (reference).dump(4) } });
				};
			}
			else if constexpr (std::is_same_v<T, Texture*>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ { value,  reference == nullptr ? "" : reference->name } });
				};
			}
			else if constexpr (std::is_same_v<T, ShaderProperties>)
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ { value,  "{}" } });
				};
			}
			else
			{
				jsonCallback = [&](std::string value) {
					return nlohmann::json({ {value, reference } });
				};
			}
		}

		std::pair<std::string, ManifestTuple> newTuplePair(fieldName, { defaultValue, initCallback, jsonCallback });
		config.insert(newTuplePair);
	}
}