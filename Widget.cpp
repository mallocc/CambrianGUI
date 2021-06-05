#include "Widget.h"
#include "GUI.h"
#include "Utilities.h"
//#include "editor.h"
#include "ShaderProperties.h"
#include <inttypes.h>
#include <GL/glew.h>

using gui::Widget;

namespace
{
#ifdef USE_FBO
	struct ScopedFBO
	{
		float w, h;
		ScopedFBO(float w, float h, GLuint fboId) : w(w), h(h)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glViewport(0, 0, w, h);
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		~ScopedFBO()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, NULL);
			glViewport(0, 0, w, h);/*
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
		}
	};
#endif
}


inline float fmodFixed(float a, float b)
{
	return fmod(fmod(a, b) + b, b);
}

void gui::Widget::radioUp(std::string event)
{
	if (m_radio)
		if (m_radioParent != nullptr)
		{
			nlohmann::json j;
			j["intent"] = "radio";
			j["id"] = m_id;
			j["event"] = event;
			j["meta"] = m_meta;
			m_radioParent->onIntent(j);
		}
}

Widget* gui::Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)
{
	bool handled = false;

	if (m_clickable)
	{
		if (onOverEvent(mouseEventData, process) || focus)
		{
			if (process)
			{
				onOver(m_gui, mouseEventData);
			}

			onMoveEvent(mouseEventData, process);

			if (onDownEvent(mouseEventData, process))
			{
				if (process)
				{
					onDown(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onRightClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onRightClick(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onClick(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onDragEvent(mouseEventData, process))
			{
				if (process)
				{
					onDrag(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onDoubleClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onDoubleClick(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onReleaseEvent(mouseEventData, process))
			{
				if (process)
				{
					onRelease(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (onUpEvent(mouseEventData, process))
			{
				if (process)
				{
					onUp(m_gui, mouseEventData);
				}
			}

			if (onMiddleClickEvent(mouseEventData, process))
			{
				if (process)
				{
					onMiddleClick(m_gui, mouseEventData);
				}
				handled = true;
			}

			if (process)
			{
				m_gui->setCursor(m_cursor);
			}
		}
		else if (onLeaveEvent(mouseEventData, process))
		{
			if (process)
			{
				onLeave(m_gui, mouseEventData);
			}
		}

		if (process)
		{
			m_oldMouseEventData = mouseEventData;
		}
	}

	return handled && !m_clickThrough ? this : nullptr;
}

bool gui::Widget::keyDown(int virtualKey, KeyEventData* keys)
{
	return (keys->keyboardState[virtualKey] & 128 && !(m_oldKeyEventData.keyboardState[virtualKey] & 128));
}

bool gui::Widget::keyUp(int virtualKey, KeyEventData* keys)
{
	return (!(keys->keyboardState[virtualKey] & 128) && (m_oldKeyEventData.keyboardState[virtualKey] & 128));
}

Widget* gui::Widget::onKeyEvent(KeyEventData keyEventData)
{
	m_oldKeyEventData = keyEventData;
	return nullptr;
}

void gui::Widget::draw(float tx, float ty, bool editMode)
{
	tx += m_x; ty += m_y;

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


	{
#ifdef USE_FBO
		ScopedFBO fbo(getGUI()->w, getGUI()->h, getGUI()->m_frameBufferHandle);
#endif
		// DRAW STUFF
		{
			if (m_depth > 0.001f)
			{

				GLuint prog = getGUI()->getShaderManger()->getShader("assets/shaders/boxblur");
				float largerSide = std::max(W(), H());
				glUseProgram(prog);
				//glUniform2f(glGetUniformLocation(prog, "pos"), tx + largerSide * m_depth * 10, ty + largerSide * m_depth * 10);
				glUniform2f(glGetUniformLocation(prog, "pos"), tx, ty);
				glUniform2f(glGetUniformLocation(prog, "size"), W(), H());
				glUniform2f(glGetUniformLocation(prog, "guisize"), getGUI()->w, getGUI()->h);
				glUniform1f(glGetUniformLocation(prog, "radius"), m_roundedRadius / largerSide / 2.0f);
				glUniform1f(glGetUniformLocation(prog, "depth"), m_depth);
				glUniform4f(glGetUniformLocation(prog, "color_map"), m_shadowColor.r, m_shadowColor.g, m_shadowColor.b, m_shadowColor.a);
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				glPushMatrix();
				//glTranslatef(tx, ty, 0);

				glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex2f(0, 0);
				glTexCoord2f(1, 0);
				glVertex2f(getGUI()->w, 0);
				glTexCoord2f(1, 1);
				glVertex2f(getGUI()->w, getGUI()->h);
				glTexCoord2f(0, 1);
				glVertex2f(0, getGUI()->h);
				glEnd();

				glEnd();
				glPopMatrix();

				glUseProgram(0);
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
			}

			if (m_shape == "image")
				if (m_background != nullptr)
				{
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);
					if (m_blendMode == "screen")
					{
						glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
					}
					else
					{
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					glBindTexture(GL_TEXTURE_2D, m_background->id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glColor4f(m_color.r, m_color.g, m_color.b, m_opacity *
						((m_background != m_backgroundTransition) ? m_backgroundTransitionValue : 1.0f));
					glPushMatrix();
					glTranslatef(tx, ty, 0);
					glTranslatef(m_w / 2.0f, m_h / 2.0f, 0);
					glRotatef(m_rotation, 0, 0, 1);
					glTranslatef(-m_w / 2.0f, -m_h / 2.0f, 0);
					glScalef(m_w, m_h, 1);
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

			if (m_shape == "rect")
			{
				bool gradIsVert = m_gradient == "vertical";
				bool gradIsHori = m_gradient == "horizontal";
				if (m_roundedRadius > 0.0f)
				{
					//if (m_color.a > 0.0f)
					{

						glPushMatrix();
						{

							glTranslatef(tx, ty, 0);

							glBegin(GL_POLYGON);
							{
								glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);

								//top
								if (gradIsVert || gradIsHori)
									glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
								glVertex2f(m_roundedRadius, 0);

								if (gradIsHori)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								glVertex2f(W() - m_roundedRadius, 0);

								constexpr float quarterCircle = 3.14159f / 2.0f;
								constexpr float inc = quarterCircle / 8;

								// top right

								float offsetRad = quarterCircle * 3.0f;
								for (float rad = offsetRad; rad <= quarterCircle + offsetRad; rad += inc)
									glVertex2f(
										cos(rad) * m_roundedRadius + W() - m_roundedRadius,
										sin(rad) * m_roundedRadius + m_roundedRadius);

								// right
								if (gradIsHori)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								if (gradIsVert)
									glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
								glVertex2f(W(), m_roundedRadius);
								if (gradIsVert)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								glVertex2f(W(), H() - m_roundedRadius);

								// bottom right
								offsetRad = 0.0f;
								for (float rad = offsetRad + quarterCircle; rad > 0.0f; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + W() - m_roundedRadius,
										cos(rad) * m_roundedRadius + H() - m_roundedRadius);

								// bottom
								if (gradIsVert)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								if (gradIsHori)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								glVertex2f(W() - m_roundedRadius, H());
								if (gradIsHori)
									glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
								glVertex2f(m_roundedRadius, H());

								// bottom left

								offsetRad = quarterCircle * 3.0f;
								for (float rad = offsetRad + quarterCircle; rad > offsetRad; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + m_roundedRadius,
										cos(rad) * m_roundedRadius + H() - m_roundedRadius);

								// left
								if (gradIsHori)
									glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
								if (gradIsVert)
									glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
								glVertex2f(0, H() - m_roundedRadius);
								if (gradIsVert)
									glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
								glVertex2f(0, m_roundedRadius);

								// top left

								offsetRad = quarterCircle * 2.0f;
								for (float rad = offsetRad + quarterCircle; rad > offsetRad; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + m_roundedRadius,
										cos(rad) * m_roundedRadius + m_roundedRadius);
							}
							glEnd();
						}
						glPopMatrix();
					}

					//if (m_borderColor.a > 0.0f)
					{
						glPushMatrix();
						{
							glColor4f(m_borderColor.r, m_borderColor.g, m_borderColor.b, m_borderColor.a);
							glLineWidth(m_borderWidth);

							glTranslatef(tx, ty, 0);

							glBegin(GL_LINE_LOOP);
							{
								glVertex2f(m_roundedRadius, 0);
								glVertex2f(W() - m_roundedRadius, 0);

								constexpr float quarterCircle = 3.14159f / 2.0f;
								constexpr float inc = quarterCircle / 8;

								float offsetRad = quarterCircle * 3.0f;
								for (float rad = offsetRad; rad <= quarterCircle + offsetRad; rad += inc)
									glVertex2f(
										cos(rad) * m_roundedRadius + W() - m_roundedRadius,
										sin(rad) * m_roundedRadius + m_roundedRadius);

								glVertex2f(W(), m_roundedRadius);
								glVertex2f(W(), H() - m_roundedRadius);

								offsetRad = 0.0f;
								for (float rad = offsetRad + quarterCircle; rad > 0.0f; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + W() - m_roundedRadius,
										cos(rad) * m_roundedRadius + H() - m_roundedRadius);

								glVertex2f(W() - m_roundedRadius, H());
								glVertex2f(m_roundedRadius, H());

								offsetRad = quarterCircle * 3.0f;
								for (float rad = offsetRad + quarterCircle; rad > offsetRad; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + m_roundedRadius,
										cos(rad) * m_roundedRadius + H() - m_roundedRadius);

								glVertex2f(0, H() - m_roundedRadius);
								glVertex2f(0, m_roundedRadius);

								offsetRad = quarterCircle * 2.0f;
								for (float rad = offsetRad + quarterCircle; rad > offsetRad; rad -= inc)
									glVertex2f(
										sin(rad) * m_roundedRadius + m_roundedRadius,
										cos(rad) * m_roundedRadius + m_roundedRadius);
							}
							glEnd();

							glLineWidth(1.0f);
						}
						glPopMatrix();
					}
				}
				else
				{
					glColor4f(m_color.r, m_color.g, m_color.b, m_color.a);

					glPushMatrix();

					glTranslatef(tx, ty, 0);
					glScalef(W(), H(), 1);

					glBegin(GL_QUADS);
					{
						if (gradIsVert || gradIsHori)
							glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
						glVertex2f(0, 0);
						if (gradIsVert)
							glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
						if (gradIsHori)
							glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
						glVertex2f(1, 0);
						if (gradIsHori || gradIsVert)
							glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
						glVertex2f(1, 1);
						if (gradIsHori)
							glColor4f(m_colorStart.r, m_colorStart.g, m_colorStart.b, m_colorStart.a);
						if (gradIsVert)
							glColor4f(m_colorEnd.r, m_colorEnd.g, m_colorEnd.b, m_colorEnd.a);
						glVertex2f(0, 1);
					}
					glEnd();

					glColor4f(m_borderColor.r, m_borderColor.g, m_borderColor.b, m_borderColor.a);

					glLineWidth(m_borderWidth);
					glBegin(GL_LINE_LOOP);
					{
						glVertex2f(0, 0);
						glVertex2f(1, 0);
						glVertex2f(1, 1);
						glVertex2f(0, 1);
					}
					glEnd();
					glLineWidth(1.0f);

					glPopMatrix();
				}
			}

			if (m_background != m_backgroundTransition)
				if (m_backgroundTransition != nullptr)
				{
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);
					if (m_blendMode == "screen")
					{
						glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
					}
					else
					{
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					glBindTexture(GL_TEXTURE_2D, m_backgroundTransition->id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glColor4f(m_color.r, m_color.g, m_color.b, m_opacity * (1.0f - m_backgroundTransitionValue));
					glPushMatrix();
					glTranslatef(tx, ty, 0);
					glRotatef(m_rotation, 0, 0, 1);
					glScalef(m_w, m_h, 1);
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

			if (m_debugMode == DebugMode::DBG_BOUNDS)// && gui->editedWidget == this)
			{
				glLineWidth(2);
				if (m_over)
					glColor3f(0, 1, 0);
				else
					glColor3f(1, 0, 0);

				glPushMatrix();
				glTranslatef(tx, ty, 0);
				glScalef(m_w, m_h, 1);
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
		}
	}

#ifdef USE_FBO
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, 17);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPushMatrix();
		glTranslatef(0, getGUI()->h, 0);
		glScalef(getGUI()->w, -getGUI()->h, 0);
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
#endif
}

void gui::Widget::revalidate()
{
	if (m_scaled)
	{
		if (m_background != nullptr)
		{
			m_wTarget = m_background->width;
			m_hTarget = m_background->height;
		}
	}

	m_rotation += (m_rotationTarget - m_rotation) * m_transitionSpeed;
	fmodFixed(m_rotation, 360.0f);

	if (std::fabs(m_xTarget - m_x) > 0.1f)
		m_x += (m_xTarget - m_x) * m_transitionSpeed;
	if (std::fabs(m_yTarget - m_y) > 0.1f)
		m_y += (m_yTarget - m_y) * m_transitionSpeed;
	if (std::fabs(m_wTarget - m_w) > 0.1f)
		m_w += (m_wTarget - m_w) * m_transitionSpeed;
	if (std::fabs(m_hTarget - m_h) > 0.1f)
		m_h += (m_hTarget - m_h) * m_transitionSpeed;

	if (m_background != m_backgroundTransition)
	{
		m_backgroundTransitionValue += m_transitionSpeed;
		if (m_backgroundTransitionValue >= 1.0f)
		{
			m_backgroundTransition = m_background;
			m_backgroundTransitionValue = 0.0f;
		}
	}

	lerpColor(m_targetColor, m_color, m_transitionSpeed);
	lerpColor(m_targetBorderColor, m_borderColor, m_transitionSpeed);
	lerpColor(m_targetColorStart, m_colorStart, m_transitionSpeed);
	lerpColor(m_targetColorEnd, m_colorEnd, m_transitionSpeed);
	lerpColor(m_targetShadowColor, m_shadowColor, m_transitionSpeed);

	m_depth += (m_targetDepth - m_depth) * m_transitionSpeed;
}

bool gui::Widget::init(const nlohmann::json& j, bool ignoreType)
{
	m_defaultJson = j;
	bool success = true;
	if (checkJSON(j, "widget"))
	{
		ConfigList fields;
		{
			fields["widget"] = m_type;
			fields["meta"] = m_meta;
			fields["shell-execute"] = m_shellExecute;
			fields["blend-mode"] = { m_blendMode      , "normal" };
			fields["opacity"] = { m_opacity        , "1.0" };
			fields["transition-speed"] = { m_transitionSpeed, "1.0" };
			fields["x"] = m_xTarget;
			fields["y"] = m_yTarget;
			fields["x-offset"] = m_xOffset;
			fields["y-offset"] = m_yOffset;
			fields["w"] = m_wTarget;
			fields["h"] = m_hTarget;
			fields["rotation"] = m_rotationTarget;
			fields["hint"] = m_hint;
			fields["visible"] = { m_visible        , "true" };
			fields["exclusive-envoke"] = m_exclusiveEnvoke;
			fields["clickable"] = { m_clickable      , "true" };
			fields["click-through"] = m_clickThrough;
			fields["scaled"] = m_scaled;
			fields["centered"] = m_centered;
			fields["proportional"] = m_proportional;
			fields["id"] = m_id;
			fields["weight"] = { m_weight, "0.0" };
			fields["on-over"] = m_onOverJson;
			fields["on-leave"] = m_onLeaveJson;
			fields["on-release"] = m_onReleaseJson;
			fields["on-click"] = m_onClickJson;
			fields["on-checked"] = m_onCheckedJson;
			fields["on-unchecked"] = m_onUncheckedJson;
			fields["on-click-external"] = m_onClickExternalJson;
			fields["on-release-external"] = m_onReleaseExternalJson;
			fields["on-checked-external"] = m_onCheckedExternalJson;
			fields["on-unchecked-external"] = m_onUncheckedExternalJson;
			fields["cursor"] = m_cursor;
			fields["checked"] = m_checked;
			fields["check-on-click"] = m_checkOnClick;
			fields["checkable"] = m_checkable;
			fields["radio"] = m_radio;
			fields["background"] = textureConfigItem(m_background);
			fields["shader"] = shaderPropertiesConfigItem(m_shaderProperties);
			fields["color"] = colorConfigItem(m_targetColor, "#0000");
			fields["color-start"] = colorConfigItem(m_targetColorStart, "#0000");
			fields["color-end"] = colorConfigItem(m_targetColorEnd, "#0000");
			fields["background-tiled"] = m_backgroundTiled;
			fields["omit"] = m_layoutOmit;
			fields["border-color"] = colorConfigItem(m_targetBorderColor, "#0000");
			fields["border-width"] = { m_borderWidth, "1" };
			fields["shape"] = m_shape;
			fields["radius"] = { m_roundedRadius, "0" };
			fields["gradient"] = { m_gradient, "none" };
			fields["depth"] = m_targetDepth;
			fields["shadow-color"] = colorConfigItem(m_targetShadowColor, "#000");
		}
		fields.load(j);

		m_config += fields;

		if (m_background != nullptr)
			m_shape = "image";
		m_backgroundTransition = m_background;
		m_x = m_xTarget;
		m_y = m_yTarget;
		m_w = m_wTarget;
		m_h = m_hTarget;
		m_rotation = m_rotationTarget;
		m_color = m_targetColor;
		m_borderColor = m_targetBorderColor;
		m_colorStart = m_targetColorStart;
		m_colorEnd = m_targetColorEnd;
		m_shadowColor = m_targetShadowColor;
		m_depth = m_targetDepth;
		if (m_scaled)
		{
			if (m_background != nullptr)
			{
				m_wTarget = m_background->width;
				m_hTarget = m_background->height;
			}
		}

		if (m_checked)
		{
			m_config.load(m_onCheckedJson, true);
		}
		else
		{
			m_config.load(m_onUncheckedJson, true);
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
	return m_config.toJson();
}

void gui::Widget::getAbsolutePosition(float& xPos, float& yPos)
{
	xPos += m_x;
	yPos += m_y;
	if (m_parent != nullptr)
	{
		m_parent->getAbsolutePosition(xPos, yPos);
	}
}

gui::Widget::Widget(GUI* gui) : m_gui(gui)/*, debugMode(DebugMode::DBG_BOUNDS)*/
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
	if (mouseEventData.leftDown && !m_oldMouseEventData.leftDown)
	{
		if (process)
		{
			m_initialMouseEventData = mouseEventData;
			if (m_shellExecute != "")
			{
				system(m_shellExecute.c_str());
			}
			m_gui->fireTriggers(m_onClickJson, this);

			m_config.load(m_onClickJson, true);

			if (m_checkOnClick)
				if (m_checkable || m_radio)
				{
					toggleCheck();
				}

			m_gui->getWidgetManager()->handleDynamicJson(m_onClickExternalJson, m_id);
		}
		return true;
	}
	return false;
}

bool gui::Widget::onRightClickEvent(MouseEventData mouseEventData, bool process)
{
	if (mouseEventData.rightDown && !m_oldMouseEventData.rightDown)
	{
		return true;
	}
	return false;
}

bool gui::Widget::onDoubleClickEvent(MouseEventData mouseEventData, bool process)
{
	// Handle click event
	if (mouseEventData.leftDown && !m_oldMouseEventData.leftDown)
	{
		// Handle double click event
		auto timeNow = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - m_oldClickTime).count();
		if (process)
		{
			m_oldClickTime = timeNow;
		}
		if (elapsed < GetDoubleClickTime())
		{
			// Do stuff for double click
			m_oldClickTime = {};
			return true;
		}
	}
	return false;
}
bool gui::Widget::onMoveEvent(MouseEventData mouseEventData, bool process)
{
	if ((mouseEventData.x != m_oldMouseEventData.x || mouseEventData.y != m_oldMouseEventData.y))
	{
		if (process)
		{
			m_gui->showHintLabel(m_hint, m_hintShowTime);
		}
		return true;
	}
	return false;
}
bool gui::Widget::onDragEvent(MouseEventData mouseEventData, bool process)
{
	// Handle drag event
	if (mouseEventData.leftDown && m_oldMouseEventData.leftDown &&
		(mouseEventData.x != m_oldMouseEventData.x || mouseEventData.y != m_oldMouseEventData.y))
	{
		// Cancel waiting double click
		m_oldClickTime = {};
		return true;
	}
	return false;
}

bool gui::Widget::onReleaseEvent(MouseEventData mouseEventData, bool process)
{
	// Handle released event
	if (!mouseEventData.leftDown && m_oldMouseEventData.leftDown && m_down)
	{
		if (process)
		{
			m_config.load(m_onReleaseJson, true);

			if (!m_checkOnClick)
				if (m_checkable || m_radio)
				{
					toggleCheck();
				}

			m_gui->getWidgetManager()->handleDynamicJson(m_onReleaseExternalJson, m_id);
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
	if (mouseEventData.x > tx && mouseEventData.x < tx + m_w &&
		mouseEventData.y > ty && mouseEventData.y < ty + m_h)
	{
		if (process)
		{
			if (!m_over && !m_checked)
			{
				m_config.load(m_onOverJson, true);
			}
			m_over = true;
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
	if (mouseEventData.x < tx || mouseEventData.x > tx + m_w ||
		mouseEventData.y < ty || mouseEventData.y > ty + m_h)
	{
		if (process)
		{
			if (m_over)
			{
				if (!m_checked)
					m_config.load(m_onLeaveJson, true);
				m_gui->hideHintLabel();
			}
			m_over = false;
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
			m_down = true;
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
			m_down = false;
		}
		return true;
	}
	return false;
}

bool gui::Widget::onMiddleClickEvent(MouseEventData mouseEventData, bool process)
{
	// Handle middle clicked event
	if (mouseEventData.middleDown && !m_oldMouseEventData.middleDown)
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
	tx += m_x;
	ty += m_y;
	if (m_parent != nullptr)
		m_parent->getContextPosition(tx, ty);
}

void gui::Widget::check(bool updatedRadio, bool force)
{
	if (!m_checked || force)
	{
		if (updatedRadio)
			radioUp("check");
		m_gui->fireTriggers(m_onCheckedJson, this);
		m_config.load(m_onCheckedJson, true);
		m_gui->getWidgetManager()->handleDynamicJson(m_onCheckedExternalJson, m_id);
		onChecked(m_gui, m_oldMouseEventData);
		m_checked = true;
	}
}

void gui::Widget::uncheck(bool updatedRadio, bool force)
{
	if (m_checked || force)
	{
		m_gui->fireTriggers(m_onUncheckedJson, this);
		m_config.load(m_onUncheckedJson, true);
		m_gui->getWidgetManager()->handleDynamicJson(m_onUncheckedExternalJson, m_id);
		onUnchecked(m_gui, m_oldMouseEventData);
		m_checked = false;
		if (updatedRadio)
			radioUp("uncheck");
	}
}

void gui::Widget::toggleCheck(bool updatedRadio, bool force)
{
	if (m_checked)
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
	return m_checked;
}

bool gui::Widget::getColor(std::string colorName, Color& color)
{
	return m_gui->getGUIConfig()->getColor(colorName, color);
}

std::string gui::Widget::getColorName(Color& color)
{
	return m_gui->getGUIConfig()->getColorName(color);
}

gui::Texture* gui::Widget::requireTexture(std::string path)
{
	return  m_gui->getTextureManager()->requireTexture(path);
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

ConfigList& gui::Widget::getConfig()
{
	return m_config;
}

nlohmann::json& gui::Widget::getDefaultJson()
{
	return m_defaultJson;
}

gui::MouseEventData& gui::Widget::getMouseEventData()
{
	return m_oldMouseEventData;
}

void gui::Widget::setMouseEventData(MouseEventData& mouseEventData)
{
	m_oldMouseEventData = mouseEventData;
}

gui::MouseEventData& gui::Widget::getInitialMouseEventData()
{
	return m_initialMouseEventData;
}

void gui::Widget::setInitialMouseEventData(MouseEventData& mouseEventData)
{
	m_initialMouseEventData = mouseEventData;
}

gui::KeyEventData& gui::Widget::getKeyEventData()
{
	return m_oldKeyEventData;
}

void gui::Widget::setDebugMode(DebugMode debugMode)
{
	this->m_debugMode = debugMode;
}

gui::Widget::DebugMode gui::Widget::getDebugMode()
{
	return m_debugMode;
}

gui::GUI* gui::Widget::getGUI()
{
	return m_gui;
}

void gui::Widget::setX(float x, bool force)
{
	m_xTarget = x;
	if (force)
		this->m_x = m_xTarget;
}

float gui::Widget::getX()
{
	return m_x;
}

float gui::Widget::X()
{
	return m_x;
}

float gui::Widget::getTargetX()
{
	return m_xTarget;
}

void gui::Widget::setY(float y, bool force)
{
	m_yTarget = y;
	if (force)
		this->m_y = m_yTarget;

}

float gui::Widget::getY()
{
	return m_y;
}

float gui::Widget::Y()
{
	return m_y;
}

float gui::Widget::getTargetY()
{
	return m_yTarget;
}

void gui::Widget::setOffsetX(float x)
{
	m_xOffset = x;
}

float gui::Widget::getOffsetX()
{
	return m_xOffset;
}

void gui::Widget::setOffsetY(float y)
{
	m_yOffset = y;
}

float gui::Widget::getOffsetY()
{
	return m_yOffset;
}

void gui::Widget::setW(float w, bool force)
{
	m_wTarget = w;
	if (force)
		this->m_w = m_wTarget;

}

float gui::Widget::getW()
{
	return m_w;
}

float* gui::Widget::getTargetWRef()
{
	return &m_wTarget;
}

float gui::Widget::W()
{
	return m_w;
}

float gui::Widget::getTargetW()
{
	return m_wTarget;
}

void gui::Widget::setH(float h, bool force)
{
	m_hTarget = h;
	if (force)
		this->m_h = m_hTarget;

}

float gui::Widget::getH()
{
	return m_h;
}

float gui::Widget::H()
{
	return m_h;
}

float gui::Widget::getTargetH()
{
	return m_hTarget;
}

void gui::Widget::setWeight(float weight)
{
	this->m_weight = weight;
}

float gui::Widget::getWeight()
{
	return m_weight;
}

void gui::Widget::setRotation(float angle, bool force)
{
	m_rotationTarget = angle;
	if (force)
		this->m_rotation = m_rotationTarget;

}

float gui::Widget::getRotation()
{
	return m_rotation;
}

float gui::Widget::getTargetRotation()
{
	return m_rotationTarget;
}

void gui::Widget::setProportional(bool proportional)
{
	this->m_proportional = proportional;
}

bool gui::Widget::isProportional()
{
	return m_proportional;
}

void gui::Widget::setCentered(bool centered)
{
	this->m_centered = centered;
}

bool gui::Widget::isCentered()
{
	return m_centered;
}

void gui::Widget::setLayoutOmit(bool layoutOmit)
{
	this->m_layoutOmit = layoutOmit;
}

bool gui::Widget::isOmitFromLayout()
{
	return m_layoutOmit;
}

void gui::Widget::setBackground(Texture* background)
{
	this->m_background = background;
}

gui::Texture* gui::Widget::getBackground()
{
	return m_background;
}

void gui::Widget::setTransitionSpeed(float speed)
{
	this->m_transitionSpeed = speed;
}

float gui::Widget::getTransitionSpeed()
{
	return m_transitionSpeed;
}

void gui::Widget::setBackgroundTiled(bool tiled)
{
	this->m_backgroundTiled = tiled;
}

bool gui::Widget::isBackgroundTiled()
{
	return m_backgroundTiled;
}

void gui::Widget::setParent(Widget* parent)
{
	this->m_parent = parent;
}

Widget* gui::Widget::getParent()
{
	return m_parent;
}

void gui::Widget::setId(std::string id)
{
	this->m_id = id;
}

std::string gui::Widget::getId()
{
	return m_id;
}

void gui::Widget::setBlendMode(std::string mode)
{
	this->m_blendMode = mode;
}

std::string gui::Widget::getBlendMode()
{
	return m_blendMode;
}

void gui::Widget::setShellExecute(std::string shell)
{
	this->m_shellExecute = shell;
}

std::string gui::Widget::getShellExecute()
{
	return m_shellExecute;
}

void gui::Widget::setHint(std::string hint)
{
	this->m_hint = hint;
}

std::string gui::Widget::getHint()
{
	return m_hint;
}

void gui::Widget::setOpacity(float opacity)
{
	this->m_opacity = opacity;
}

float gui::Widget::getOpacity()
{
	return m_opacity;
}

void gui::Widget::setMeta(nlohmann::json meta)
{
	this->m_meta = meta;
}

nlohmann::json gui::Widget::getMeta()
{
	return m_meta;
}

void gui::Widget::setColor(Color color, bool force)
{
	m_targetColor = color;
	if (force)
		this->m_color = m_targetColor;

}

gui::Color gui::Widget::getColor()
{
	return m_color;
}

gui::Color gui::Widget::getTargetColor()
{
	return m_targetColor;
}

void gui::Widget::setCheckOnClick(bool checkOnClick)
{
	this->m_checkOnClick = checkOnClick;
}

bool gui::Widget::isCheckOnClick()
{
	return m_checkOnClick;
}

void gui::Widget::setExclusiveEnvoke(bool exclusiveEnvoke)
{
	this->m_exclusiveEnvoke = exclusiveEnvoke;
}

bool gui::Widget::isExclusiveEnvoke()
{
	return m_exclusiveEnvoke;
}

float gui::Widget::getBackgroundTransitionValue()
{
	return m_backgroundTransitionValue;
}

std::string gui::Widget::getType()
{
	return m_type;
}

void gui::Widget::setVisible(bool visible)
{
	this->m_visible = visible;
}

void gui::Widget::show()
{
	m_visible = true;
}

void gui::Widget::hide()
{
	m_visible = false;
}

bool gui::Widget::isVisible()
{
	return m_visible;
}

void gui::Widget::setClickable(bool clickable)
{
	this->m_clickable = clickable;
}

bool gui::Widget::isClickable()
{
	return m_clickable;
}

void gui::Widget::setClickThrough(bool clickThrough)
{
	this->m_clickThrough = clickThrough;
}

bool gui::Widget::isClickThrough()
{
	return m_clickThrough;
}

void gui::Widget::setScaled(bool scaled)
{
	this->m_scaled = scaled;
}

bool gui::Widget::isScaled()
{
	return m_scaled;
}

void gui::Widget::setCheckable(bool checkable)
{
	this->m_checkable = checkable;
}

bool gui::Widget::isCheckable()
{
	return m_checkable;
}

void gui::Widget::setRadio(bool radio)
{
	this->m_radio = radio;
}

bool gui::Widget::isRadio()
{
	return m_radio;
}

void gui::Widget::setRadioParent(Widget* radioParent)
{
	this->m_radioParent = radioParent;
}

Widget* gui::Widget::getRadioParent()
{
	return m_radioParent;
}

void gui::Widget::setCursor(std::string cursor)
{
	this->m_cursor = cursor;
}

std::string gui::Widget::getCursor()
{
	return m_cursor;
}

bool gui::Widget::isDown()
{
	return m_down;
}

bool gui::Widget::isOver()
{
	return m_over;
}
