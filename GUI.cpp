#include "GUI.h"

#include "HLayout.h"
#include "DropdownList.h"
#include "Dropdown.h"
#include "Button.h"
#include "TextEdit.h"
#include "GridLayout.h"
#include "ScrollLayout.h"
#include "TabLayout.h"
#include "Slider.h"
#include "HSlider.h"
#include "VSlider.h"
#include "Spinner.h"
#include "FormLayout.h"

#include <sstream>
#include <fstream>

#include <shellapi.h>
#include <FileWatch.h>

using gui::GUI;
using gui::FontManager;
using gui::TextureManager;
using gui::Widget;
using gui::Color;

namespace gui
{
	const std::map<std::string, LPSTR> cursors = {
		{"hand", IDC_HAND},
		{"cross", IDC_CROSS},
		{"help", IDC_HELP},
		{"ibeam", IDC_IBEAM},
		{"sizeall", IDC_SIZEALL},
		{"sizenesw", IDC_SIZENESW},
		{"sizens", IDC_SIZENS},
		{"sizenwse", IDC_SIZENWSE},
		{"sizewe", IDC_SIZEWE},
		{"wait", IDC_WAIT},
		{"no", IDC_NO}
	};
}

void gui::GUI::resize(float w, float h)
{
	this->w = w;
	this->h = h;
	// Draw children
	Widget* rootWidget = widgetManager->getRootWidget();
	if (rootWidget != nullptr && rootWidget->isVisible())
	{
		rootWidget->revalidate();
	}
}

void GUI::draw()
{
	if (enableDrawing)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		const std::lock_guard<std::mutex> lock(m_mutex);

		// Draw children
		Widget* rootWidget = widgetManager->getRootWidget();
		if (rootWidget != nullptr && rootWidget->isVisible())
		{
			rootWidget->revalidate();
			rootWidget->draw(0, 0, editMode);
		}

		if (currentShowTimeHintLabel > 0UL)
		{
			--currentShowTimeHintLabel;
			if (currentShowTimeHintLabel <= 0UL)
			{
				currentShowTimeHintLabel = 0UL;
				hideHintLabel();
			}
		}
		if (currentShowTimeFloatingLabel > 0UL)
		{
			--currentShowTimeFloatingLabel;
			if (currentShowTimeFloatingLabel <= 0UL)
			{
				currentShowTimeFloatingLabel = 0UL;
				hideFloatingLabel();
			}
		}

		//Widget* dlWidget = widgetManager->getDropDownListWidget();
		//if (dlWidget != nullptr && dlWidget->isVisible())
		//{
		//	dlWidget->revalidate();
		//	dlWidget->draw(0, 0);
		//}

		//Widget* floatingLabelWidget = widgetManager->getFloatingLabelWidget();
		//if (floatingLabelWidget != nullptr && floatingLabelWidget->isVisible())
		//{
		//	floatingLabelWidget->revalidate();
		//	floatingLabelWidget->draw(0, 0);
		//}
	}
	frameCount++;
}

nlohmann::json gui::GUI::toJson()
{
	nlohmann::json j;
	//j["w"] = w;
	//j["h"] = h;
	//j["fonts"] = fonts;
	//j["colors"] = colors;

	//std::vector<nlohmann::json> instances;

	//j["classes"] = classes;
	//j["mainLayout"] = mainLayout->toJson();
	return j;
}

SplitTimer::Timer* gui::GUI::getTimer()
{
	return &timer;
}

void GUI::onMouseEvent(MouseEventData mouseEventData)
{
	//std::cout << mouseEventData.x << " " << mouseEventData.y << " left: " << mouseEventData.leftDown << " middle: " << mouseEventData.middleDown << " right: " << mouseEventData.rightDown << std::endl;

	// Look for a new focused widget in UI
	if (focusedWidget == nullptr)
	{
		Widget* rootWidget = widgetManager->getRootWidget();

		Widget* dropdownListWidget = widgetManager->getDropDownListWidget();
		if (dropdownListWidget != nullptr)
		{
			// If there is a current dropdown open: 
			//		this make the rest of the UI disabled until finsihed with the dropdown
			if (dropdownListWidget->isVisible())
			{
				// Process dropdown list anyway for hover animation etc
				dropdownListWidget->onMouseEvent(mouseEventData, true);
				// Now trigger callbacks for the parent that the dropdown opened for
				if (dropdownListWidget->getParent() != nullptr)
				{
					dropdownListWidget->getParent()->onMouseEvent(mouseEventData, true);
				}

				// If the dropdown wasnt clicked but something else was: close the dropdown
				if (!(dropdownListWidget->onOverEvent(mouseEventData, false) &&
					!dropdownListWidget->onClickEvent(mouseEventData, false)) &&
					rootWidget->onMouseEvent(mouseEventData, false) != nullptr)
				{
					closeDropdownIntent();
				}
			}
			else // If there is no currnet dropdown open, carry on as usual
			{
				// Check for event
				if (rootWidget != nullptr)
					focusedWidget = rootWidget->onMouseEvent(mouseEventData, true, false);
			}
		}
	}
	else // Theres already a widget being focuced/interacted with, so only process it and freeze disable everything else
	{
		focusedWidget->onMouseEvent(mouseEventData, /*!editMode*/ true, true);
	}

	// Reset focused widget when mouse released
	if ((!mouseEventData.leftDown) && focusedWidget != nullptr)
	{
		focusedWidget = nullptr;
	}

	// Update last handled widget
	if (focusedWidget != nullptr)
		lastHandledWidget = focusedWidget;

	// Update last mouse state
	oldMouseEventData = mouseEventData;
}

void gui::GUI::onMouseLeave()
{
	MouseEventData mouseEventData;
	mouseEventData.x = mouseEventData.y = -1;
	Widget* rootWidget = widgetManager->getRootWidget();
	if (rootWidget)
		rootWidget->onLeaveEvent(mouseEventData, true);
}

void GUI::onKeyEvent(KeyEventData keyEventData)
{
	Widget* rootWidget = widgetManager->getRootWidget();
	if (rootWidget != nullptr)
		rootWidget->onKeyEvent(keyEventData);

	if (keyDown(VK_F4, &keyEventData))
	{
		bool oldEditMode = editMode;
		editMode = !editMode;
	}
	if (keyDown(VK_F8, &keyEventData))
	{
		std::cout << toJson().dump(4) << std::endl;
	}

	if (keyEventData.tabDown && !oldKeyEventData.tabDown)
	{
		init();
	}

	oldKeyEventData = keyEventData;
}

bool gui::GUI::keyDown(int virtualKey, KeyEventData* keys)
{
	return (keys->keyboardState[virtualKey] & 128 && !(oldKeyEventData.keyboardState[virtualKey] & 128));
}

bool gui::GUI::keyUp(int virtualKey, KeyEventData* keys)
{
	return (!(keys->keyboardState[virtualKey] & 128) && (oldKeyEventData.keyboardState[virtualKey] & 128));
}

void GUI::init(std::string configOverridePath, bool firstLoad_)
{
	enableDrawing = true;
	firstLoad = firstLoad_;
	timer.reset();

	if (firstLoad)
	{
		texMan->clear();

#ifdef SHADERS
		timer.split("Textures Cleared");
		shaderManager->flushShaders();
		timer.split("Shaders Flushed");
		shaderManager->getShader("assets/shaders/default");
		defaultNormalMap = texMan->requireTexture("JP/default_normalmap.png");

		cubemap = texMan->loadCubemap({
			"assets/tex/JP/cubemap/px.png",
			"assets/tex/JP/cubemap/nx.png",
			"assets/tex/JP/cubemap/py.png",
			"assets/tex/JP/cubemap/ny.png",
			"assets/tex/JP/cubemap/pz.png",
			"assets/tex/JP/cubemap/nz.png"
			});

		timer.split("Loaded Cubemap");
#endif
		std::string fontPath = "assets/fonts/Open_Sans/OpenSans-Regular.ttf";
		fontMan->defaultFont = new Font(&fontMan->ft, fontPath, "OpenSans@24", 24, this);
		timer.split("Loaded Default Font");
	}

	try
	{
		config->loadConfig(configOverridePath.empty() ? "assets/guiconfig.json" : configOverridePath, *this);
		w = config->w;
		h = config->h;

		widgetManager->init();

		//std::ifstream t(ExePath() + "\\..\\..\\Vst_Plugins\\assets\\guiconfig.json");
		//std::string str((std::istreambuf_iterator<char>(t)),
		//std::istreambuf_iterator<char>());
		//std::string str(filesys["assets/guiconfig.json"].data, filesys["assets/guiconfig.json"].size);

		//if (configOverridePath != "")
		//{
		//	std::cout << "Loaded Overridden config: " << configOverridePath << std::endl;
		//	std::ifstream t(configOverridePath);
		//	str = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		//}
		//timer.split("Loaded Config from image");
		//nlohmann::json j = nlohmann::json::parse(str);
		//timer.split("Parsed Config to JSON structure");

		//std::string tmp;
		//bool success = true;


#ifdef USE_FBO
		{
			//Generate the framebuffer ID
			if (m_frameBufferHandle != 0)
				glDeleteFramebuffers(1, &m_frameBufferHandle);
			if (m_textureHandle != 0)
				glDeleteTextures(1, &m_textureHandle);
			glGenFramebuffers(1, &m_frameBufferHandle);// Always check that our framebuffer is ok
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Failed to generate frame buffer.";

			//Bind it while we configure it
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
			//Generation of the texture
			glGenTextures(1, &m_textureHandle);
			//Bind it
			glBindTexture(GL_TEXTURE_2D, m_textureHandle);

			//Actually create a space
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA,
				w,
				h,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				NULL);

			//Wrapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Set it as a framebuffer texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureHandle, 0);

			//Creating render object
			glGenRenderbuffers(1, &m_renderHandle);
			//Bind it
			glBindRenderbuffer(GL_RENDERBUFFER, m_renderHandle);
			//Configure it
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderHandle);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Failed to generate frame buffer.";
			//Back to the default framebuffer
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
#endif

		find_widget_as(Label, buildDateLabel, "builddatelabel")
		{
			buildDateLabel->text = "Build Date: " + std::string(__DATE__) + " @ " + std::string(__TIME__);
		}

		timer.split("Loaded Main Layout");
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		getc(stdin);
	}
	catch (nlohmann::json::exception& e)
	{
		std::cout << e.what() << std::endl;
		getc(stdin);
	}
	/*catch (...)
	{
		std::cout << "unknown exception" << std::endl;
	}*/

	runWorkerThread();
}

void gui::GUI::loadDimensions(std::string configOverridePath)
{
	try
	{
		std::string str(filesys["assets/guiconfig.json"].data, filesys["assets/guiconfig.json"].size);

		if (configOverridePath != "")
		{
			std::cout << "Loaded Overridden config: " << configOverridePath << std::endl;
			std::ifstream t(configOverridePath);
			str = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
		timer.split("Loaded Config from image");
		nlohmann::json j = nlohmann::json::parse(str);
		timer.split("Parsed Config to JSON structure");

		std::string tmp;
		bool success = true;
		success &= readJSON(j, "w", w);
		success &= readJSON(j, "h", h);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		getc(stdin);
	}
	catch (nlohmann::json::exception& e)
	{
		std::cout << e.what() << std::endl;
		getc(stdin);
	}
}

void gui::GUI::fireTriggers(nlohmann::json triggerJson, Widget* from)
{
	for (auto& i : triggerJson.items())
	{
		if (i.key() == "trigger")
		{
			if (triggerCallbacks.find(i.value()) != triggerCallbacks.end())
			{
				triggerCallbacks.at(i.value())(this, from);
			}
			break;
		}
	}
}

void gui::GUI::free()
{
	//AssetPacker::deleteFileImageMap(&filesys);
	//for (auto& i : filesys)
	//	i.second.free();
}

GUI::GUI(int32_t w, int32_t h)
	: w(w), h(h),
	oldClickTime(),
	focusedWidget(nullptr),
	oldMouseEventData(),
	focusedMouseEventData(),
	oldKeyEventData()
{
	configureOverride = "";
	configInvalidated = 0;

	if (AssetPacker::findImageInFile(DLLPath(), filesys, 1))
	{
		std::cout << "Loaded Virtual Filesystem: " << DLLPath() << std::endl;
	}
	lastEditTime = GetTickCount();
	fontMan = new FontManager(this);
	texMan = new TextureManager(&filesys);
	shaderManager = new ShaderManager(&filesys);
	config = new Configuration(&filesys);
	widgetManager = new WidgetManager(this, config);

	widgetManager->registerWidget<Layout>();
	widgetManager->registerWidget<HLayout>();
	widgetManager->registerWidget<HSlider>();
	widgetManager->registerWidget<VSlider>();
	widgetManager->registerWidget<VLayout>();
	widgetManager->registerWidget<Slider>();
	widgetManager->registerWidget<GridLayout>();
	widgetManager->registerWidget<ScrollLayout>();
	widgetManager->registerWidget<TabLayout>();
	widgetManager->registerWidget<Button>();
	widgetManager->registerWidget<Label>();
	widgetManager->registerWidget<TextEdit>();
	widgetManager->registerWidget<Dropdown>();
	widgetManager->registerWidget<List>();
	widgetManager->registerWidget<DropdownList>();
	widgetManager->registerWidget<Spinner>();
	widgetManager->registerWidget<FormLayout>();

	registerTriggerCallback("show_credits", [&](GUI* gui, Widget*) { gui->displayCredits = true; });
	registerTriggerCallback("hide_credits", [&](GUI* gui, Widget*) { gui->displayCredits = false; });
	registerTriggerCallback("test", [&](GUI*, Widget* from) { std::cout << "Triggered from widget: " << from->getId() << std::endl; });
}

GUI* gui::GUI::getGUI()
{
	return this;
}

TextureManager* GUI::getTextureManager()
{
	return texMan;
}
FontManager* GUI::getFontManager()
{
	return fontMan;
}

ShaderManager* gui::GUI::getShaderManger()
{
	return shaderManager;
}

gui::WidgetManager* gui::GUI::getWidgetManager()
{
	return widgetManager;
}

gui::Configuration* gui::GUI::getGUIConfig()
{
	return config;
}

void gui::GUI::showCursor()
{
	if (cursorVisibility < 0)
	{
		cursorVisibility = ShowCursor(true);
	}
}

void gui::GUI::hideCursor()
{
	if (cursorVisibility >= 0)
	{
		cursorVisibility = ShowCursor(false);
	}
}

void gui::GUI::setCursor(std::string cursor)
{
	if (cursor != "")
	{
		LPSTR idc = IDC_ARROW;

		if (cursors.find(cursor) != cursors.end())
		{
			idc = cursors.at(cursor);
		}

		hCursor = LoadCursor(NULL, idc);

		//std::cout << " changed to cursor: " << cursor << std::endl;
		SetCursor(hCursor);
	}
}

void gui::GUI::openDropdownIntent(Widget* parent, nlohmann::json j, intentcallback_t selectionCallback)
{
	DropdownList* dropdownListWidget = dynamic_cast<DropdownList*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr)
	{
		dropdownListWidget->show();
		dropdownListWidget->floating = false;
		dropdownListWidget->setW(parent->W(), FORCE);
		dropdownListWidget->setParent(parent);
		dropdownListWidget->initList(j);
		dropdownListWidget->onSelection = selectionCallback;
		dropdownListWidget->setExpandFlags(EXPAND_RESET);
		dropdownListWidget->setExpandFlags(EXPAND_PREFERED);
		widgetManager->bringToFront(dropdownListWidget);
	}
	else
	{
		std::cout << "Dropdown list is not configured" << std::endl;
	}
}

void gui::GUI::openRightClickIntent(nlohmann::json j, intentcallback_t selectionCallback)
{
	DropdownList* dropdownListWidget = dynamic_cast<DropdownList*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr)
	{
		dropdownListWidget->floating = true;
		dropdownListWidget->show();
		dropdownListWidget->setW(100);
		dropdownListWidget->initList(j);
		dropdownListWidget->setX(oldMouseEventData.x);
		dropdownListWidget->setY(oldMouseEventData.y);
		dropdownListWidget->onSelection = selectionCallback;
		dropdownListWidget->setExpandFlags(EXPAND_RESET);
		dropdownListWidget->setExpandFlags(EXPAND_PREFERED_HEIGHT);
		widgetManager->bringToFront(dropdownListWidget);
	}
	else
	{
		std::cout << "Dropdown list is not configured" << std::endl;
	}
}

void gui::GUI::closeDropdownIntent()
{
	//std::cout << "Closing dropdown list" << std::endl;
	DropdownList* dropdownListWidget = dynamic_cast<DropdownList*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr)
	{
		dropdownListWidget->hide();
		dropdownListWidget->floating = false;
		widget_as(Dropdown, dropdown, dropdownListWidget->getParent())
		{
			dropdown->closeDropdown();
		}
		dropdownListWidget->setParent(nullptr);
		dropdownListWidget->onSelection = [](nlohmann::json) {};
	}
}

void gui::GUI::showFloatingLabel(int32_t x, int32_t y, std::string text, uint64_t showTime)
{
	Label* floatingLabelWidget = dynamic_cast<Label*>(widgetManager->getFloatingLabelWidget());
	if (floatingLabelWidget != nullptr)
	{
		std::cout << "Showing floating label: " << text << " " << x << ":" << y << " " << showTime << std::endl;
		currentShowTimeFloatingLabel = showTime;

		floatingLabelWidget->show();
		floatingLabelWidget->text = text;
		floatingLabelWidget->setX(x);
		floatingLabelWidget->setY(y);
		widgetManager->bringToFront(floatingLabelWidget);
	}
}

void gui::GUI::hideFloatingLabel()
{
	std::cout << "Hiding floating label" << std::endl;

	Label* floatingLabelWidget = dynamic_cast<Label*>(widgetManager->getFloatingLabelWidget());
	if (floatingLabelWidget != nullptr)
	{
		currentShowTimeFloatingLabel = 0;
		floatingLabelWidget->text = "";
		floatingLabelWidget->hide();
	}
}

void gui::GUI::showHintLabel(std::string text, uint64_t showTime)
{
	if (text != "")
	{
		find_widget_as(Label, widget, "hintlabel")
		{
			currentShowTimeHintLabel = showTime;
			widget->show();
			widget->text = text;
		}
	}
}

void gui::GUI::hideHintLabel()
{
	find_widget_as(Label, widget, "hintlabel")
	{
		widget->hide();
		widget->text = "";
	}
}

void gui::GUI::registerTriggerCallback(std::string triggerName, TriggerCallback function)
{
	triggerCallbacks[triggerName] = function;
}

gui::Widget* gui::GUI::createWidget(nlohmann::json j)
{
	return getWidgetManager()->createWidget(j);
}
bool gui::GUI::removeWidget(std::string id)
{
	return getWidgetManager()->removeWidget(id);
}

void gui::GUI::runWorkerThread()
{
	std::thread thread([=]()
		{
			while (true)
			{
				if (!m_workerQueue.empty())
				{
					auto& workerCallback = m_workerQueue.front();
					const std::lock_guard<std::mutex> lock(m_mutex);
					workerCallback();
					m_workerQueue.pop();
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		});
	thread.detach();
}


void gui::GUI::runWorker(WorkerCallback workerCallback)
{
	m_workerQueue.push(workerCallback);
}