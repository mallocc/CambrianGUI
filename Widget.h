#pragma once

#include <chrono>
#include <functional>
#include <nlohmann/json.hpp>

#include "Color.h"
#include "EventData.h"
#include "TextureManager.h"
#include "ShaderProperties.h"
#include "ConfigList.h"

namespace gui
{
	class GUI;

	typedef std::function<void(gui::GUI*, MouseEventData)> callback_t;
	typedef std::function<void(gui::GUI*, KeyEventData)> keycallback_t;

	typedef std::function<void(nlohmann::json)> intentcallback_t;


	template<typename T>
	class WidgetType
	{
	public:
		static std::string getWidgetType()
		{
			return T::getWidgetType();
		}
	};

#define FORCE true
#define LERP false


#define DEFINE_WIDGET_TYPE(WIDGET_TYPENAME_SYMBOL) \
public: \
	static std::string getWidgetType() { return WIDGET_TYPENAME_SYMBOL; }

	class Widget : public WidgetType<Widget>
	{
		DEFINE_WIDGET_TYPE("widget");
	public:

		GUI* getGUI();


		virtual void setX(float x, bool force = false);
		virtual float getX();
		virtual float X();
		float getTargetX();
		virtual void setY(float y, bool force = false);
		virtual float getY();
		virtual float Y();
		float getTargetY();
		virtual void setOffsetX(float x);
		virtual float getOffsetX();
		virtual void setOffsetY(float y);
		virtual float getOffsetY();
		virtual void setW(float w, bool force = false);
		virtual float getW();
		virtual float W();
		float getTargetW();
		virtual float* getTargetWRef();
		virtual void setH(float h, bool force = false);
		virtual float getH();
		virtual float H();
		float getTargetH();
		virtual void setWeight(float weight);
		virtual float getWeight();
		virtual void setRotation(float angle, bool force = false);
		virtual float getRotation();
		float getTargetRotation();
		void setProportional(bool proportional = true);
		bool isProportional();
		void setCentered(bool centered = true);
		bool isCentered();
		void setLayoutOmit(bool layoutOmit = true);
		bool isOmitFromLayout();
		virtual void setBackground(Texture* background);
		virtual Texture* getBackground();
		void setTransitionSpeed(float speed);
		float getTransitionSpeed();
		void setBackgroundTiled(bool tiled = true);
		bool isBackgroundTiled();
		virtual void setParent(Widget* parent);
		virtual Widget* getParent();
		void setId(std::string id);
		std::string getId();
		void setBlendMode(std::string mode);
		std::string getBlendMode();
		virtual void setShellExecute(std::string shell);
		virtual std::string getShellExecute();
		virtual void setHint(std::string hint);
		virtual std::string getHint();
		virtual void setOpacity(float opacity = 1.0f);
		virtual float getOpacity();
		virtual void setMeta(nlohmann::json meta);
		virtual nlohmann::json getMeta();
		virtual void setColor(Color color, bool force = false);
		virtual Color getColor();
		Color getTargetColor();
		void setCheckOnClick(bool checkOnClick = true);
		bool isCheckOnClick();
		void setExclusiveEnvoke(bool exclusiveEnvoke = true);
		bool isExclusiveEnvoke();

		float getBackgroundTransitionValue();
		std::string getType();






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







		virtual void setVisible(bool visible = true);
		virtual void show();
		virtual void hide();
		virtual bool isVisible();
		void setClickable(bool clickable = true);
		bool isClickable();
		void setClickThrough(bool clickThrough = true);
		bool isClickThrough();
		void setScaled(bool scaled = true);
		bool isScaled();
		void setCheckable(bool checkable = true);
		bool isCheckable();
		void setRadio(bool radio = true);
		bool isRadio();
		virtual void setRadioParent(Widget* radioParent);
		virtual Widget* getRadioParent();
		void setCursor(std::string cursor);
		std::string getCursor();

		virtual bool isDown();
		virtual bool isOver();



		virtual void radioUp(std::string event);




		virtual Widget* onMouseEvent(MouseEventData mouseEventData, bool process = true, bool focus = false);
		virtual Widget* onKeyEvent(KeyEventData keyEventData);
		virtual void draw(float tx, float ty, bool editMode = false);
		virtual void revalidate();
		virtual bool init(const nlohmann::json& j, bool ignoreType = false);
		virtual nlohmann::json toJson();

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

		virtual void check(bool updatedRadio = true, bool force = false);
		virtual void uncheck(bool updatedRadio = true, bool force = false);
		virtual void toggleCheck(bool updatedRadio = true, bool force = false);
		virtual bool isChecked();

		bool getColor(std::string colorName, Color& color);
		std::string getColorName(Color& color);
		Texture* requireTexture(std::string path);
		ShaderProperties getShaderProperties(std::string value);

		template<typename W>
		bool checkWidgetType(bool ignoreCheck = false)
		{
			return WidgetType<W>::getWidgetType() == m_type || ignoreCheck;
		}


		ConfigItem colorConfigItem(Color& reference, std::string defaultValue = "");

		typedef Texture* TexturePtr;
		ConfigItem textureConfigItem(TexturePtr& reference, std::string defaultValue = "");

		ConfigItem shaderPropertiesConfigItem(ShaderProperties& reference, std::string defaultValue = "");

		enum class DebugMode
		{
			DBG_OFF,
			DBG_BOUNDS,
		} m_debugMode = DebugMode::DBG_OFF;
		void setDebugMode(DebugMode debugMode);
		DebugMode getDebugMode();

		ConfigList& getConfig();
		nlohmann::json& getDefaultJson();

		MouseEventData& getMouseEventData();
		void setMouseEventData(MouseEventData& mouseEventData);
		MouseEventData& getInitialMouseEventData();
		void setInitialMouseEventData(MouseEventData& mouseEventData);
		KeyEventData& getKeyEventData();
	protected:
		ConfigList m_config;

	private:
		GUI* m_gui;
		float m_x = 0, m_y = 0;
		float m_xOffset, m_yOffset = 0;
		float m_xTarget = 0, m_yTarget = 0;
		float m_w = 0, m_h = 0;
		float m_wTarget = 0, m_hTarget = 0;
		float m_weight = 1.0f;
		bool  m_proportional = false;
		bool  m_centered = false;
		bool  m_layoutOmit = false;
		float m_rotation = 0.0f;
		float m_rotationTarget = 0.0f;
		Texture* m_background = nullptr;
		Texture* m_backgroundTransition = m_background;
		float m_backgroundTransitionValue = 0.0f;
		float m_transitionSpeed = 1.0f;
		bool m_backgroundTiled = false;
		std::string m_type = "undefined";
		Widget* m_parent = nullptr;
		MouseEventData m_oldMouseEventData;
		MouseEventData m_initialMouseEventData;
		KeyEventData m_oldKeyEventData;
		std::chrono::time_point<std::chrono::steady_clock> m_oldClickTime;
		std::chrono::time_point<std::chrono::steady_clock> m_oldHoverTime;
		uint64_t m_hintShowTime = 100UL;
		std::string m_id = "widget";
		std::string m_blendMode = "normal";
		std::string m_shellExecute = "";
		std::string m_hint = "";
		float m_opacity = 1.0f;
		nlohmann::json m_defaultJson;
		nlohmann::json m_onOverJson;
		nlohmann::json m_onLeaveJson;
		nlohmann::json m_onClickJson;
		nlohmann::json m_onReleaseJson;
		nlohmann::json m_onCheckedJson;
		nlohmann::json m_onUncheckedJson;
		nlohmann::json m_onClickExternalJson;
		nlohmann::json m_onReleaseExternalJson;
		nlohmann::json m_onCheckedExternalJson;
		nlohmann::json m_onUncheckedExternalJson;
		nlohmann::json m_meta;
		Color m_color = { 1, 1, 1 };
		Color m_targetColor = m_color;
		bool m_checkOnClick = false;
		bool m_down = false;
		bool m_over = false;
		bool m_exclusiveEnvoke = false;
		bool m_visible = true;
		bool m_clickable = true;
		bool m_clickThrough = true;
		bool m_scaled = false;
		bool m_checked = false;
		bool m_checkable = false;
		bool m_radio = false;
		Widget* m_radioParent = nullptr;
		std::string m_cursor = "";
		ShaderProperties m_shaderProperties;


		std::string m_shape = "image";
		Color m_borderColor = black;
		Color m_targetBorderColor = m_borderColor;
		float m_borderWidth = 1.0f;
		float m_roundedRadius = 5.0f;

		std::string m_gradient = "none";
		Color m_colorStart = { 1, 1, 1 };
		Color m_targetColorStart = { 1, 1, 1 };
		Color m_colorEnd = { 1, 1, 1 };
		Color m_targetColorEnd = { 1, 1, 1 };

	};
}