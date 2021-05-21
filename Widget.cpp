#include "Widget.h"
#include "GUI.h"
#include "Utilities.h"
//#include "editor.h"
#include "ShaderProperties.h"
#include <inttypes.h>
#include <GL/glew.h>

using gui::Widget;

inline float fmodFixed(float a, float b)
{
	return fmod(fmod(a, b) + b, b);
}

void gui::Widget::radioUp(std::string event)
{
	if (radio)
		if (radioParent != nullptr)
		{
			nlohmann::json j;
			j["intent"] = "radio";
			j["id"] = widgetId;
			j["event"] = event;
			j["meta"] = metaData;
			radioParent->onIntent(j);
		}
}

Widget* gui::Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	bool handled = false;

	if (clickable)
	{
		if (onOverEvent(mouseEventData, process) || focus)
		{
			if (process)
			{
				onOver(gui, mouseEventData);
			}

			onMoveEvent(mouseEventData, process);

			if (onDownEvent(mouseEventData, process))
			{
				if (process)
				{
					onDown(gui, mouseEventData);
				}
				handled = true;
			}

			if (onRightClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onRightClick(gui, mouseEventData);
				}
				handled = true;
			}

			if (onClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onClick(gui, mouseEventData);
				}
				handled = true;
			}

			if (onDragEvent(mouseEventData, process))
			{
				if (process)
				{
					onDrag(gui, mouseEventData);
				}
				handled = true;
			}

			if (onDoubleClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onDoubleClick(gui, mouseEventData);
				}
				handled = true;
			}

			if (onReleaseEvent(mouseEventData, process))
			{
				if (process)
				{
					onRelease(gui, mouseEventData);
				}
				handled = true;
			}

			if (onUpEvent(mouseEventData, process))
			{
				if (process)
				{
					onUp(gui, mouseEventData);
				}
			}

			if (onMiddleClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onMiddleClick(gui, mouseEventData);
				}
				handled = true;
			}

			if (process)
			{
				gui->setCursor(cursor);
			}
		}
		else if (onLeaveEvent(mouseEventData, process))
		{
			if (process)
			{
				onLeave(gui, mouseEventData);
			}
		}

		if (process)
		{
			oldMouseEventData = mouseEventData;
		}
	}

	return handled && !clickThrough ? this : nullptr;
}

bool gui::Widget::keyDown(int virtualKey, KeyEventData* keys)
{
	return (keys->keyboardState[virtualKey] & 128 && !(oldKeyEventData.keyboardState[virtualKey] & 128));
}

bool gui::Widget::keyUp(int virtualKey, KeyEventData* keys)
{
	return (!(keys->keyboardState[virtualKey] & 128) && (oldKeyEventData.keyboardState[virtualKey] & 128));
}

Widget* gui::Widget::onKeyEvent(KeyEventData keyEventData)
{
	oldKeyEventData = keyEventData;
	return nullptr;
}

void gui::Widget::draw(float tx, float ty, bool editMode)
{
	tx += x; ty += y;

#ifdef SHADERS
	if (shaderProperties.shader != "" && shaderProperties.texture != "")
	{
		glEnable(GL_MULTISAMPLE);
		glColor4f(.05, .05, .05, 1);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gui->getTextureManager()->requireTexture(shaderProperties.texture)->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		float aspect = w / h;

		//Can we not just do this in the actual shader by multiplying gl_TexCoord[0].st by that value?

		float wd = shaderProperties.iterations * aspect;
		float hd = shaderProperties.iterations;

		GLuint prog = gui->getShaderManger()->getShader("assets/shaders/" + shaderProperties.shader);

		glUseProgram(prog);
		glUniform1i(glGetUniformLocation(prog, "tex"), 0);
		glUniform1f(glGetUniformLocation(prog, "currentTime"), gui->frameCount);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, gui->getTextureManager()->requireTexture(shaderProperties.texture)->id);

		glPushMatrix();
		glTranslatef(tx, ty, 0);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(wd, 0);
		glVertex2f(w, 0);
		glTexCoord2f(wd, hd);
		glVertex2f(w, h);
		glTexCoord2f(0, hd);
		glVertex2f(0, h);
		glEnd();

		glEnd();
		glPopMatrix();

		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
	}
#endif

	if (background != nullptr)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		if (blendMode == "screen")
		{
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		}
		else
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glBindTexture(GL_TEXTURE_2D, background->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glColor4f(color.r, color.g, color.b, opacity *
			((background != backgroundTransition) ? backgroundTransitionValue : 1.0f));
		glPushMatrix();
		glTranslatef(tx, ty, 0);
		glTranslatef(w / 2.0f, h / 2.0f, 0);
		glRotatef(rotation, 0, 0, 1);
		glTranslatef(-w / 2.0f, -h / 2.0f, 0);
		glScalef(w, h, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(1, 0);
			glVertex2f(1, 0);
			glTexCoord2f(1, 1);
			glVertex2f(1, 1);
			glTexCoord2f(0, 1);
			glVertex2f(0, 1);
		}
		glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, NULL);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	//else
	//{
	//	glColor4f(color.r, color.g, color.b, opacity);
	//	glPushMatrix();
	//	glTranslatef(tx, ty, 0);
	//	glScalef(w, h, 1);
	//	glBegin(GL_QUADS);
	//	{
	//		glVertex2f(0, 0);
	//		glVertex2f(1, 0);
	//		glVertex2f(1, 1);
	//		glVertex2f(0, 1);
	//	}
	//	glEnd();
	//	glPopMatrix();
	//}

	if (background != backgroundTransition)
		if (backgroundTransition != nullptr)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			if (blendMode == "screen")
			{
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			}
			else
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glBindTexture(GL_TEXTURE_2D, backgroundTransition->id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glColor4f(color.r, color.g, color.b, opacity * (1.0f - backgroundTransitionValue));
			glPushMatrix();
			glTranslatef(tx, ty, 0);
			glRotatef(rotation, 0, 0, 1);
			glScalef(w, h, 1);
			glBegin(GL_QUADS);
			{
				glTexCoord2f(0, 0);
				glVertex2f(0, 0);
				glTexCoord2f(1, 0);
				glVertex2f(1, 0);
				glTexCoord2f(1, 1);
				glVertex2f(1, 1);
				glTexCoord2f(0, 1);
				glVertex2f(0, 1);
			}
			glEnd();
			glPopMatrix();

			glBindTexture(GL_TEXTURE_2D, NULL);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}

	if (debugMode == DebugMode::DBG_BOUNDS)// && gui->editedWidget == this)
	{
		glLineWidth(2);
		if (over)
			glColor3f(0, 1, 0);
		else
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
		glLineWidth(1);
	}

	glColor4f(1, 1, 1, 1);
}

void gui::Widget::revalidate()
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

	if (scaled)
	{
		if (background != nullptr)
		{
			wTarget = background->width;
			hTarget = background->height;
		}
	}

	rotation += (rotationTarget - rotation) * transitionSpeed;
	fmodFixed(rotation, 360.0f);

	x += (xTarget - x) * transitionSpeed;
	y += (yTarget - y) * transitionSpeed;
	w += (wTarget - w) * transitionSpeed;
	h += (hTarget - h) * transitionSpeed;

	if (background != backgroundTransition)
	{
		backgroundTransitionValue += transitionSpeed;
		if (backgroundTransitionValue >= 1.0f)
		{
			backgroundTransition = background;
			backgroundTransitionValue = 0.0f;
		}
	}

	color.r += (targetColor.r - color.r) * transitionSpeed;
	color.g += (targetColor.g - color.g) * transitionSpeed;
	color.b += (targetColor.b - color.b) * transitionSpeed;
}

bool gui::Widget::init(nlohmann::json j, bool ignoreType)
{
	defaultJson = j;
	bool success = true;
	if (checkJSON(j, "widget"))
	{
		ConfigList fields;
		{
			fields["widget"]                = type;
			fields["meta"]                  = metaData;
			fields["shell-execute"]         = shellExecute;
			fields["blend-mode"]            = { blendMode      , "normal" };
			fields["opacity"]               = { opacity        , "1.0" };
			fields["transition-speed"]      = { transitionSpeed, "1.0" };
			fields["x"]                     = xTarget;
			fields["y"]                     = yTarget;
			fields["x-offset"]              = xOffset;
			fields["y-offset"]              = yOffset;
			fields["w"]                     = wTarget;
			fields["h"]                     = hTarget;
			fields["rotation"]              = rotationTarget;
			fields["sensitivity"]           = { sensitivity    , "300" };
			fields["text"]                  = text;
			fields["hint"]                  = hint;
			fields["visible"]               = { visible        , "true" };
			fields["exclusive-envoke"]      = exclusiveEnvoke;
			fields["clickable"]             = { clickable      , "true" };
			fields["click-through"]         = clickThrough;
			fields["scaled"]                = scaled;
			fields["centered"]              = centered;
			fields["proportional"]          = proportional;
			fields["id"]                    = widgetId;
			fields["weight"]                = weight;
			fields["on-over"]               = onOverJson;
			fields["on-leave"]              = onLeaveJson;
			fields["on-release"]            = onReleaseJson;
			fields["on-click"]              = onClickJson;
			fields["on-checked"]            = onCheckedJson;
			fields["on-unchecked"]          = onUncheckedJson;
			fields["on-click-external"]     = onClickExternalJson;
			fields["on-release-external"]   = onReleaseExternalJson;
			fields["on-checked-external"]   = onCheckedExternalJson;
			fields["on-unchecked-external"] = onUncheckedExternalJson;
			fields["cursor"]                = cursor;
			fields["checked"]               = checked;
			fields["check-on-click"]        = checkOnClick;
			fields["checkable"]             = checkable;
			fields["radio"]                 = radio;
			fields["background"]            = textureConfigItem(background);
			fields["shader-properties"]     = shaderPropertiesConfigItem(shaderProperties);
			fields["color"]                 = colorConfigItem(targetColor);
			fields["color-start"]           = colorConfigItem(colorStart);
			fields["color-end"]             = colorConfigItem(colorEnd);
		}
		fields.load(j);

		config += fields;

		backgroundTransition = background;
		x = xTarget;
		y = yTarget;
		w = wTarget;
		h = hTarget;
		rotation = rotationTarget;
		color = targetColor;

		if (w == 0)
		{
			if (parent != nullptr)
			{
				wTarget = parent->w;
			}
			else
			{
				wTarget = gui->w;
			}
		}
		if (h == 0)
		{
			if (parent != nullptr)
			{
				hTarget = parent->h;
			}
			else
			{
				hTarget = gui->h;
			}
		}

		if (scaled)
		{
			if (background != nullptr)
			{
				wTarget = background->width;
				hTarget = background->height;
			}
		}

		if (checked)
		{
			config.load(onCheckedJson, true);
		}
		else
		{
			config.load(onUncheckedJson, true);
		}
	}
	else
	{
		success = false;
		std::cout << "There were errors in reading config for Widget" << std::endl;
	}
	return success;
}

nlohmann::json gui::Widget::toJson()
{
	return config.toJson();
}

void gui::Widget::getAbsolutePosition(float& xPos, float& yPos)
{
	xPos += x;
	yPos += y;
	if (parent != nullptr)
	{
		parent->getAbsolutePosition(xPos, yPos);
	}
}

gui::Widget::Widget(GUI* gui) : gui(gui)/*, debugMode(DebugMode::DBG_BOUNDS)*/
{
	onUp = [](gui::GUI* gui, MouseEventData mouseEventData) {/* std::cout << "up" << std::endl;*/ };
	onDown = [](gui::GUI* gui, MouseEventData mouseEventData) {/* std::cout << "down" << std::endl;*/ };
	onOver = [](gui::GUI* gui, MouseEventData mouseEventData) { /* std::cout << "over" << std::endl;*/ };
	onLeave = [](gui::GUI* gui, MouseEventData mouseEventData) {/* std::cout << "leave" << std::endl;*/ };
	onClick = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "click" << std::endl;*/ };
	onRelease = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "release" << std::endl;*/ };
	onDoubleClick = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "double clicked" << std::endl;*/ };
	onDrag = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "dragged" << std::endl; */};
	onMiddleClick = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "dragged" << std::endl; */};
	onRightClick = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "dragged" << std::endl; */};
	onChecked = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "dragged" << std::endl; */};
	onUnchecked = [](gui::GUI* gui, MouseEventData mouseEventData) { /*std::cout << "dragged" << std::endl; */};
}

bool gui::Widget::onClickEvent(MouseEventData mouseEventData, bool process)
{
	// Handle clicked event
	if (mouseEventData.leftDown && !oldMouseEventData.leftDown)
	{
		if (process)
		{
			initialMouseEventData = mouseEventData;
			if (shellExecute != "")
			{
				system(shellExecute.c_str());
			}
			gui->fireTriggers(onClickJson);

			config.load(onClickJson, true);

			if (checkOnClick)
				if (checkable || radio)
				{
					toggleCheck();
				}

			gui->getWidgetManager()->handleDynamicJson(onClickExternalJson, widgetId);
		}
		return true;
	}
	return false;
}

bool gui::Widget::onRightClickEvent(MouseEventData mouseEventData, bool process)
{
	if (mouseEventData.rightDown && !oldMouseEventData.rightDown)
	{
		return true;
	}
	return false;
}

bool gui::Widget::onDoubleClickEvent(MouseEventData mouseEventData, bool process)
{
	// Handle click event
	if (mouseEventData.leftDown && !oldMouseEventData.leftDown)
	{
		// Handle double click event
		auto timeNow = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - oldClickTime).count();
		if (process)
		{
			oldClickTime = timeNow;
		}
		if (elapsed < GetDoubleClickTime())
		{
			// Do stuff for double click
			oldClickTime = {};
			return true;;
		}
	}
	return false;
}
bool gui::Widget::onMoveEvent(MouseEventData mouseEventData, bool process)
{
	if ((mouseEventData.x != oldMouseEventData.x || mouseEventData.y != oldMouseEventData.y))
	{
		if (process)
		{
			gui->showHintLabel(hint, hintShowTime);
		}
		return true;
	}
	return false;
}
bool gui::Widget::onDragEvent(MouseEventData mouseEventData, bool process)
{
	// Handle drag event
	if (mouseEventData.leftDown && oldMouseEventData.leftDown &&
		(mouseEventData.x != oldMouseEventData.x || mouseEventData.y != oldMouseEventData.y))
	{
		return true;
	}
	return false;
}

bool gui::Widget::onReleaseEvent(MouseEventData mouseEventData, bool process)
{
	// Handle released event
	if (!mouseEventData.leftDown && oldMouseEventData.leftDown && down)
	{
		if (process)
		{
			config.load(onReleaseJson, true);

			if (!checkOnClick)
				if (checkable || radio)
				{
					toggleCheck();
				}

			gui->getWidgetManager()->handleDynamicJson(onReleaseExternalJson, widgetId);
		}
		return true;
	}
	return false;
}

bool gui::Widget::onOverEvent(MouseEventData mouseEventData, bool process)
{
	float tx = 0, ty = 0;
	getContextPosition(tx, ty);

	// if over component
	if (mouseEventData.x > tx && mouseEventData.x < tx + w &&
		mouseEventData.y > ty && mouseEventData.y < ty + h)
	{
		if (process)
		{
			if (!over && !checked)
			{
				config.load(onOverJson, true);
			}
			over = true;
		}
		return true;
	}
	return false;
}

bool gui::Widget::onLeaveEvent(MouseEventData mouseEventData, bool process)
{
	float tx = 0, ty = 0;
	getContextPosition(tx, ty);

	// if over component
	if (mouseEventData.x < tx || mouseEventData.x > tx + w ||
		mouseEventData.y < ty || mouseEventData.y > ty + h)
	{
		if (process)
		{
			if (over)
			{
				if (!checked)
					config.load(onLeaveJson, true);
				gui->hideHintLabel();
			}
			over = false;
		}
		return true;
	}
	return false;
}

bool gui::Widget::onDownEvent(MouseEventData mouseEventData, bool process)
{
	// Handle down event
	if (mouseEventData.leftDown)
	{
		if (process)
		{
			down = true;
		}
		return true;
	}
	return false;
}

bool gui::Widget::onUpEvent(MouseEventData mouseEventData, bool process)
{
	// Handle up event
	if (!mouseEventData.leftDown)
	{
		if (process)
		{
			down = false;
		}
		return true;
	}
	return false;
}

bool gui::Widget::onMiddleClickEvent(MouseEventData mouseEventData, bool process)
{
	// Handle middle clicked event
	if (mouseEventData.middleDown && !oldMouseEventData.middleDown)
	{
		return true;
	}
	return false;
}

void gui::Widget::onIntent(nlohmann::json intent)
{
}

void gui::Widget::getContextPosition(float& tx, float& ty)
{
	tx += x;
	ty += y;
	if (parent != nullptr)
		parent->getContextPosition(tx, ty);
}

void gui::Widget::check(bool updatedRadio, bool force)
{
	if (!checked || force)
	{
		if (updatedRadio)
			radioUp("check");
		gui->fireTriggers(onCheckedJson);
		config.load(onCheckedJson, true);
		gui->getWidgetManager()->handleDynamicJson(onCheckedExternalJson, widgetId);
		onChecked(gui, oldMouseEventData);
		checked = true;
	}
}

void gui::Widget::uncheck(bool updatedRadio, bool force)
{
	if (checked || force)
	{
		gui->fireTriggers(onUncheckedJson);
		config.load(onUncheckedJson, true);
		gui->getWidgetManager()->handleDynamicJson(onUncheckedExternalJson, widgetId);
		onUnchecked(gui, oldMouseEventData);
		checked = false;
		if (updatedRadio)
			radioUp("uncheck");
	}
}

void gui::Widget::toggleCheck(bool updatedRadio, bool force)
{
	if (checked)
	{
		uncheck(updatedRadio, force);
	}
	else
	{
		check(updatedRadio, force);
	}
}

bool gui::Widget::isChecked()
{
	return checked;
}

bool gui::Widget::getColor(std::string colorName, Color& color)
{
	return gui->getGUIConfig()->getColor(colorName, color);
}

std::string gui::Widget::getColorName(Color& color)
{
	return gui->getGUIConfig()->getColorName(color);
}

gui::Texture* gui::Widget::requireTexture(std::string path)
{
	return  gui->getTextureManager()->requireTexture(path);
}

gui::ShaderProperties gui::Widget::getShaderProperties(std::string value)
{
	return ShaderProperties(nlohmann::json::parse(value));
}

ConfigItem gui::Widget::colorConfigItem(Color& reference, std::string defaultValue)
{
	ConfigItem tuple;

	if (defaultValue.empty())
		defaultValue = "#ffffff";

	tuple.initCallback = [&](std::string value) {
		getColor(value, reference);
	};

	tuple.jsonCallback = [&](std::string value) {
		return nlohmann::json({ { value, getColorName(reference) } });
	};

	tuple.val = defaultValue;

	return tuple;
}

ConfigItem gui::Widget::textureConfigItem(TexturePtr& reference, std::string defaultValue)
{
	ConfigItem tuple;

	if (defaultValue.empty())
		defaultValue = "";

	tuple.initCallback = [&](std::string value) {
		reference = requireTexture(value);
	};

	tuple.jsonCallback = [&](std::string value) {
		return nlohmann::json({ { value,  reference == nullptr ? "" : reference->name } });
	};

	tuple.val = defaultValue;

	return tuple;
}

ConfigItem gui::Widget::shaderPropertiesConfigItem(ShaderProperties& reference, std::string defaultValue)
{
	ConfigItem tuple;

	if (defaultValue.empty())
		defaultValue = "{}";

	tuple.initCallback = [&](std::string value) {
		reference = getShaderProperties(value);
	};

	tuple.jsonCallback = [&](std::string value) {
		return nlohmann::json({ { value,  "{}" } });
	};

	tuple.val = defaultValue;

	return tuple;
}





