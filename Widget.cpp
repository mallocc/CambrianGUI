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

void gui::Widget::radioUp()
{
	if (radio)
		if (radioParent != nullptr)
		{
			nlohmann::json j;
			j["intent"] = "radio";
			j["id"] = widgetId;
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

	if (editMode)// && gui->editedWidget == this)
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
	config = j;
	bool success = true;
	if (checkJSON(j, "widget"))
	{
		addToManifestList(j,
			{
				{
					"widget",
					{"",
					[&](std::string value) { type = value; },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, type }}); }}
				},
				{
					"shell-execute",
					{"",
					[&](std::string value) { shellExecute = value; },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, shellExecute }}); }}
				},
				{
					"color",
					{"#ffffff",
					[&](std::string value) { gui->getGUIConfig()->getColor(value, targetColor); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(color) }}); }}
				},
				{
					"color-start",
					{"#ffffff",
					[&](std::string value) { gui->getGUIConfig()->getColor(value, colorStart); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(colorStart) }}); }}
				},
				{
					"color-end",
					{"#ffffff",
					[&](std::string value) { gui->getGUIConfig()->getColor(value, colorEnd); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, gui->getGUIConfig()->getColorName(colorEnd) }}); }}
				},
				{
					"blend-mode",
					{"normal",
					[&](std::string value) { blendMode = value; },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, blendMode }}); }}
				},
				{
					"opacity",
					{"1.0",
					[&](std::string value) { opacity = std::atof(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, opacity }}); }}
				},
				{
					"transition-speed",
					{"1.0",
					[&](std::string value) { transitionSpeed = std::atof(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, transitionSpeed }}); }}
				},
				{
					"x",
					{"0",
					[&](std::string value) { xTarget = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, x }}); }}
				},
				{
					"y",
					{"0",
					[&](std::string value) { yTarget = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, y }}); }}
				},
				{
					"x-offset",
					{"0",
					[&](std::string value) { xOffset = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, xOffset }}); }}
				},
				{
					"y-offset",
					{"0",
					[&](std::string value) { yOffset = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, yOffset }}); }}
				},
				{
					"w",
					{"0",
					[&](std::string value) { wTarget = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, w }}); }}
				},
				{
					"h",
					{"0",
					[&](std::string value) { hTarget = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, h }}); }}
				},
				{
					"rotation",
					{"0",
					[&](std::string value) { rotationTarget = std::atoi(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, rotation }}); }}
				},
				{
					"sensitivity",
					{"300.0",
					[&](std::string value) { sensitivity = std::atof(value.c_str()); sensitivityShift = sensitivity * 10.0f; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, sensitivity }}); }}
				},
				{
					"text",
					{"---",
					[&](std::string value) { text = value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, text }}); }}
				},
				{
					"hint",
					{"",
					[&](std::string value) { hint = value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, hint }}); }}
				},
				{
					"visible",
					{"true",
					[&](std::string value) { visible = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, visible }}); }}
				},
				{
					"exclusive-envoke",
					{"false",
					[&](std::string value) { exclusiveEnvoke = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, exclusiveEnvoke }}); }}
				},
				{
					"clickable",
					{"true",
					[&](std::string value) { clickable = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, clickable }}); }}
				},
				{
					"click-through",
					{"false",
					[&](std::string value) { clickThrough = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, clickThrough }}); }}
				},
				{
					"scaled",
					{"false",
					[&](std::string value) { scaled = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, scaled }}); }}
				},
				{
					"centered",
					{"false",
					[&](std::string value) { centered = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, centered }}); }}
				},
				{
					"proportional",
					{"false",
					[&](std::string value) { proportional = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, proportional }}); }}
				},
				{
					"id",
					{"",
					[&](std::string value) { widgetId = value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, widgetId }}); }}
				},
				{
					"background",
					{"",
					[&](std::string value) { background = gui->getTextureManager()->requireTexture(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, background == nullptr ? "" : background->name }}); }}
				},
				{
					"weight",
					{"0",
					[&](std::string value) { weight = std::atof(value.c_str()); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, weight }}); }}
				},
				{
					"on-over",
					{"{}",
					[&](std::string value) { onOverJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onOverJson.dump(4) }}); }}
				},
				{
					"on-leave",
					{"{}",
					[&](std::string value) { onLeaveJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onLeaveJson.dump(4) }}); }}
				},
				{
					"on-release",
					{"{}",
					[&](std::string value) { onReleaseJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onReleaseJson.dump(4) }}); }}
				},
				{
					"on-click",
					{"{}",
					[&](std::string value) { onClickJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onClickJson.dump(4) }}); }}
				},
				{
					"on-checked",
					{"{}",
					[&](std::string value) { onCheckedJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onCheckedJson.dump(4) }}); }}
				},
				{
					"on-unchecked",
					{"{}",
					[&](std::string value) { onUncheckedJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onUncheckedJson.dump(4) }}); }}
				},
				{
					"on-click-external",
					{"{}",
					[&](std::string value) { onClickExternalJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onClickExternalJson.dump(4) }}); }}
				},
				{
					"on-release-external",
					{"{}",
					[&](std::string value) { onReleaseExternalJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onReleaseExternalJson.dump(4) }}); }}
				},
				{
					"on-checked-external",
					{"{}",
					[&](std::string value) { onCheckedExternalJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onCheckedExternalJson.dump(4) }}); }}
				},
				{
					"on-unchecked-external",
					{"{}",
					[&](std::string value) { onUncheckedExternalJson = nlohmann::json::parse(value); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, onUncheckedExternalJson.dump(4) }}); }}
				},
				{
					"cursor",
					{"",
					[&](std::string value) { cursor = value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, cursor }}); }}
				},
				{
					"checked",
					{"false",
					[&](std::string value) { checked = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, checked }}); }}
				},
				{
					"checked-click",
					{"false",
					[&](std::string value) { checkOnClick = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, checkOnClick }}); }}
				},
				{
					"checkable",
					{"false",
					[&](std::string value) { checkable = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, checkable }}); }}
				},
				{
					"shader-properties",
					{"{}",
					[&](std::string value) { shaderProperties = ShaderProperties(nlohmann::json::parse(value)); },
					[&](std::string fieldName) { return nlohmann::json({{ fieldName, "" }}); }}
				},
				{
					"radio",
					{"false",
					[&](std::string value) { radio = "true" == value; },
					[&](std::string fieldName) { return nlohmann::json({{fieldName, radio}}); }}
				}
			}
		);

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
			loadManifest(onCheckedJson, manifestList, 0, true);
		}
		else
		{
			loadManifest(onUncheckedJson, manifestList, 0, true);
		}
	}
	else
	{
		success = false;
		std::cout << "There were errors in reading config for Component" << std::endl;
	}
	return success;
}

nlohmann::json gui::Widget::toJson()
{
	return manifestToJson(manifestList);
}

void gui::Widget::addToManifestList(nlohmann::json j, std::map<std::string, ManifestTuple> manifest)
{
	// TODO: remove VST dependancy
	//UtilityPluginEditor* ptr = (UtilityPluginEditor*)gui->effect->getEditor();
	//if (ptr != nullptr)
	//{
	//	wglMakeCurrent(ptr->hDC, ptr->hGLRC);
	//}

	manifestList.insert(manifest.begin(), manifest.end());
	loadManifest(j, manifest);
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

gui::Widget::Widget(GUI* gui) : gui(gui)
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

			loadManifest(onClickJson, manifestList, 0, true);

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
			loadManifest(onReleaseJson, manifestList, 0, true);

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
				loadManifest(onOverJson, manifestList, 0, true);
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
					loadManifest(onLeaveJson, manifestList, 0, true);
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
		gui->fireTriggers(onCheckedJson);
		loadManifest(onCheckedJson, manifestList, 0, true);
		gui->getWidgetManager()->handleDynamicJson(onCheckedExternalJson, widgetId);
		onChecked(gui, oldMouseEventData);
		checked = true;
		if (updatedRadio)
			radioUp();
	}
}

void gui::Widget::uncheck(bool updatedRadio, bool force)
{
	if (checked || force)
	{
		gui->fireTriggers(onUncheckedJson);
		loadManifest(onUncheckedJson, manifestList, 0, true);
		gui->getWidgetManager()->handleDynamicJson(onUncheckedExternalJson, widgetId);
		onUnchecked(gui, oldMouseEventData);
		checked = false;
		if (updatedRadio)
			radioUp();
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
