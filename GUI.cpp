#include "GUI.h"

#include "HLayoutWidget.h"
#include "DropdownListWidget.h"
#include "DropdownWidget.h"
#include "TabMenuWidget.h"

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

void GUI::draw()
{
	if (enableDrawing)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw children
		Widget* rootWidget = widgetManager->getRootWidget();
		if (rootWidget != nullptr && rootWidget->m_visible)
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

		Widget* floatingLabelWidget = widgetManager->getFloatingLabelWidget();
		if (floatingLabelWidget != nullptr && floatingLabelWidget->m_visible)
		{
			floatingLabelWidget->revalidate();
			floatingLabelWidget->draw(0, 0);
		}

		//Widget* dropdownListWidget = widgetManager->getDropDownListWidget();
		//if (dropdownListWidget != nullptr && dropdownListWidget->visible)
		//{
		//	dropdownListWidget->revalidate();
		//	dropdownListWidget->draw(0, 0);
		//}

		//if (editedWidgetInfoLabel != nullptr && editMode)
		//{
		//	editedWidgetInfoLabel->draw(oldMouseEventData.x, oldMouseEventData.y);
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

	hCursor = LoadCursor(NULL, IDC_ARROW);
	// Look for a new focused widget in children
	if (focusedWidget == nullptr)
	{
		Widget* rootWidget = widgetManager->getRootWidget();
		// Check for event
		if (rootWidget != nullptr)
			focusedWidget = rootWidget->onMouseEvent(mouseEventData, /*!editMode*/ true, false);

		if (focusedWidget != nullptr)
		{
			Widget* dropdownListWidget = widgetManager->getDropDownListWidget();
			if (dropdownListWidget != nullptr &&
				focusedWidget != dropdownListWidget->m_parent)
			{
				closeDropdownIntent();
			}
		}
	}
	else
	{
		focusedWidget->onMouseEvent(mouseEventData, /*!editMode*/ true, true);
	}

	//if (editMode)
	//{
	//	setCursor("cross");
	//	if (focusedWidget != nullptr)
	//	{
	//		if (focusedWidget->onClickEvent(mouseEventData, false))
	//		{
	//			editedWidget = focusedWidget;
	//		}
	//	}
	//}

	// Reset focused widget when mouse released
	if ((!mouseEventData.leftDown) && focusedWidget != nullptr)
	{
		focusedWidget = nullptr;
		//hideFloatingLabel();
	}

	if (focusedWidget != nullptr)
		lastHandledWidget = focusedWidget;

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

		find_widget_as(LabelWidget, buildDateLabel, "builddatelabel", this)
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

void gui::GUI::fireTriggers(nlohmann::json triggerJson)
{
	for (auto& i : triggerJson.items())
	{
		if (i.key() == "trigger")
		{
			if (triggerCallbacks.find(i.value()) != triggerCallbacks.end())
				triggerCallbacks.at(i.value())(this);
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

	widgetManager->registerWidget<HLayoutWidget>();
	widgetManager->registerWidget<VLayoutWidget>();
	widgetManager->registerWidget<LabelWidget>();
	widgetManager->registerWidget<DropdownWidget>();
	widgetManager->registerWidget<DropdownListWidget>();
	widgetManager->registerWidget<TabMenuWidget>();
	widgetManager->registerWidget<ContainerWidget>();

	registerTriggerCallback("show_credits", [&](GUI* g) { g->displayCredits = true; });
	registerTriggerCallback("hide_credits", [&](GUI* g) { g->displayCredits = false; });
	registerTriggerCallback("test", [&](GUI* g) { std::cout << "test" << std::endl; });
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
		SetCursor(hCursor);
	}
}

void gui::GUI::openDropdownIntent(Widget* parent, nlohmann::json j)
{
	DropdownListWidget* dropdownListWidget = dynamic_cast<DropdownListWidget*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr &&
		dropdownListWidget->m_parent != parent)
	{
		dropdownListWidget->m_visible = true;

		if (dropdownListWidget->m_visible)
		{
			dropdownListWidget->m_parent = parent;
			dropdownListWidget->m_w = parent->m_w;
			dropdownListWidget->initList(j);
		}

		//widgetManager->bringToFront(dropdownListWidget);
	}
	else
	{
		std::cout << "Dropdown list is not configured" << std::endl;
	}
}

void gui::GUI::openRightClickIntent(Widget* parent, nlohmann::json j)
{
	DropdownListWidget* dropdownListWidget = dynamic_cast<DropdownListWidget*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr &&
		dropdownListWidget->m_parent != parent)
	{
		dropdownListWidget->m_visible = true;

		if (dropdownListWidget->m_visible)
		{
			dropdownListWidget->m_parent = parent;
			dropdownListWidget->floating = true;
			dropdownListWidget->initList(j);
			dropdownListWidget->m_wTarget = parent->m_w;
			dropdownListWidget->m_xTarget = oldMouseEventData.x;
			dropdownListWidget->m_yTarget = oldMouseEventData.y;
		}

		//widgetManager->sendToBack(dropdownListWidget);
	}
	else
	{
		std::cout << "Dropdown list is not configured" << std::endl;
	}
}

void gui::GUI::closeDropdownIntent()
{
	DropdownListWidget* dropdownListWidget = dynamic_cast<DropdownListWidget*>(widgetManager->getDropDownListWidget());
	if (dropdownListWidget != nullptr)
	{
		nlohmann::json j;
		j["intent"] = "dropdown";
		j["event"] = "close";
		dropdownListWidget->onIntent(j);
		dropdownListWidget->m_visible = false;
		dropdownListWidget->floating = false;
		dropdownListWidget->m_parent = nullptr;
	}
}

void gui::GUI::showFloatingLabel(int32_t x, int32_t y, std::string text, uint64_t showTime)
{
	LabelWidget* floatingLabelWidget = dynamic_cast<LabelWidget*>(widgetManager->getFloatingLabelWidget());
	if (floatingLabelWidget != nullptr)
	{
		std::cout << "Showing floating label: " << text << " " << x << ":" << y << " " << showTime << std::endl;
		currentShowTimeFloatingLabel = showTime;

		floatingLabelWidget->m_visible = true;
		floatingLabelWidget->text = text;
		floatingLabelWidget->m_xTarget = x;
		floatingLabelWidget->m_yTarget = y;
	}
}

void gui::GUI::hideFloatingLabel()
{
	LabelWidget* floatingLabelWidget = dynamic_cast<LabelWidget*>(widgetManager->getFloatingLabelWidget());
	if (floatingLabelWidget != nullptr)
	{
		floatingLabelWidget->text = "";
		floatingLabelWidget->m_visible = false;
	}
}

void gui::GUI::showHintLabel(std::string text, uint64_t showTime)
{
	if (text != "")
	{
		find_widget_as(LabelWidget, widget, "hintlabel", this)
		{
			currentShowTimeHintLabel = showTime;
			widget->m_visible = true;
			widget->text = text;
		}
	}
}

void gui::GUI::hideHintLabel()
{
	find_widget_as(LabelWidget, widget, "hintlabel", this)
	{
		widget->m_visible = false;
		widget->text = "";
	}
}

void gui::GUI::registerTriggerCallback(std::string triggerName, TriggerCallback function)
{
	triggerCallbacks[triggerName] = function;
}
