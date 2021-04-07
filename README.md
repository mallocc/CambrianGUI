# ꞒambrianGUI 🦠
 This is a quick start guide to using *CambrianGUI*.
## Contents
 - Setup
 - GUI Struture
   - GUI
   - WidgetManager
   - Configration
 - Widgets
   - Widget
   - LayoutWidget
 - Event Flow
 - Configuration
 - Examples
	- Implementation
     	- Class Setup
     	- Registering a Widget
     	- Defining Event Callbacks
     	- Adding Configured Properties
     - Configuration
       - Basic Structure
       - Inline Includes
       - Colors
       - Classes
       - Global Variables
       - Local Variables
    - Other
       - Registering Callback Triggers
---
## Setup

In order to program using *CambrianGUI*, you will need:
 - GLEW installed into you project
 - Niels Lohmann's [JSON C++ Header Library](https://github.com/nlohmann/json)
 - Hugh's [SplitTimer](https://github.com/thehugh100/Split-Timer) and [AssetPacker](https://github.com/thehugh100/Simple-Asset-Packer)
 - Windows API (planning on being free from this)
 - Thomas Monkman's [FileWatch](https://github.com/ThomasMonkman/filewatch)
---
## GUI Struture

### GUI

`GUI` is the main class that contains all of the framework. 
This is where you have access to all managers and the high level calls to the framework.
The various managers can be accessed by all your widget classes.
The mains ones we are interested in are `WidgetManager` and `Configuration`.
It is advised that your implementation inherits `GUI` and overrides 
its virtual functions, though make sure to call the respective parent class methods.

`virtual void init(std::string configOverridePath, bool firstLoad);` needs to be called in your initialisation stage of your program.

`virtual void draw();` should be called in your main graphic thread, as this uses GLEW API calls.

`virtual void onMouseEvent(MouseEventData mouseEventData);` should be called to pass mouse events to the GUI. 
`MouseEventData` is just a container for the mouse button flags.
Similarly, `virtual void onKeyEvent(KeyEventData keyEventData);` should be called to pass key stroke events, where `KeyEventData` is the key button flags.

`GUI` is essentially the bootstrap for passing information to all widgets through recursive calls.

### WidgetManager

*todo*

### Configuration

*todo*

## Widgets

### Widget

`Widget` is the base class for defining a widget.
This stores widget properties such as position, size, event callbacks, etc.
`Widget` can be used as generic button by default, but would be advised to inherit and implement your own functions for draw and callback logic.

`Widget* gui::Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)` is the callback for mouse events.
This likely won't need to be reimplemented as it will call the approriate event callbacks that can be redefined later on.
The respective `callback_t` should be redefined if you want your widget to react to events and should be done in the constructor.
`virtual void draw(float tx, float ty, bool editMode = false);` is called recurisvely, so make sure to call your parent `Widget` class methods.
`virtual bool init(nlohmann::json j, bool ignoreType = false);` should be reimplemented if you require dervied class member properties linked to configuration. 
Examples will aid in how it should be implemented.

Widgets can have properties that change during runtime according to the configuration.
This is achieved by defining properties, and their data, which will be changed from another widget's events being triggered.
This causes the widget to effectivelly be reinitialised from configuration with these new properties.
Use `virtual void addToManifestList(nlohmann::json j, std::map<std::string, ManifestTuple> manifest);` in `init()` to link new properties to configuration.

### LayoutWidget

*todo*

## Event Flow

*CambrianGUI* uses an Delegate Event Model for event handling of different widgets. 
Each widget has its own event handler that uses a Event Bubbling event propagation to determine the intended widget in the DOM tree of elements.
This starts from the outermost element and recursively iterates the DOM tree.
When a widget in the tree decides that it should handle the event, it will consume it by returning `true`, so to signal to parents that the search should stop.

e.g. 

```c++
void GUI::onMouseEvent(MouseEventData mouseEventData);
``` 
 - call to the `GUI`'s root widget

```c++
Widget* LayoutWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);
```
 - root widget will most likely be a LayoutWidget, that has children widgets, which the event will be passed to
 
```c++
Widget* Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);
```
 - this is where the event is parsed. If consumed, the event will be handled by the appropriate event callback.
The function will return as `true` if consumed to notify that parent classes no longer need to handle the event,
unless custom event handling have been implemented.
 

## Configuration

*CambrianGUI* uses JSON to store the configuration of the GUI. 
 - It has a DOM tree structure, where widgets may contain lists of other children widgets.
 - JSON can be split into separate files using inline include tags in JSON objects.
 - Global properties can be defined and reference multiple times.
 - Widget classes can be defined and used as templates to create generic widgets. This should help with widgets that only vary by the color and position. Local variables can be defined inside classes if there are properties that follow a naming pattern. This can then be concatenated inline to provide a robust can highly configurable template.
 - Colors can be defined as variables that are used later in configuration, or referenced in code.
 - Fonts can be defined and used as 'font classes'.
---
## Examples
*todo*

### Implementation

#### Class Setup

It's likely you'll need to reimplement `init()` and `draw()`. 
Also, a const string should be defined for the classname; This is needed later for configuration.
```c++
const std::string MYWIDGET_CLASSNAME = "mywidget";

struct MyWidget : Widget
{
	virtual void draw(float tx, float ty, bool editMode = false);
	virtual bool init(nlohmann::json j, bool ignoreType = false);
	MyWidget(GUI* gui, nlohmann::json j);
	bool myFlag = false;
};
```

Here is the basic code for the implemented functions:

```c++
void MyWidget::draw(float tx, float ty, bool editMode)
{
	// Call parent draw function
	Widget::draw(tx, ty, editMode);
	// Update current origin to position of parent + this widgets relative position
	tx += x; ty += y;

	// draw other stuff
}

bool MyWidget::init(nlohmann::json j, bool ignoreType)
{
	bool success = false;
	
	// Check we have initialised the widget properly
	if (Widget::init(j))
	{
		// Does 'widget' match our classname 'mywidget' in configuration
		if (doesTypeMatch(ignoreType))
		{
			success = true;
		}
	}

	return success;
}

MyWidget::MyWidget(GUI* gui, nlohmann::json j) : Widget(gui)
{
	// Set our new widget's classname for verifying our widget from configuration
	setClassname(MYWIDGET_CLASSNAME);
	// Call to init
	init(j);
}
```

#### Registering a Widget

Once your widget has been created, you'll need to regiester it with your GUI implemention.
```c++
struct MyGUI : GUI
{
	MyGUI(int32_t w, int32_t h) : GUI(w, h)
	{
		getWidgetManager()->registerWidget(
						MYWIDGET_CLASSNAME, 
						[](GUI* a, nlohmann::json b) { return new MyWidget(a, b); });
	}
};
```

#### Defining Event Callbacks

#### Adding Configured Properties

### Configuration

#### Basic Structure

#### Inline Includes

#### Colors

#### Classes

#### Global Variables

#### Local Variables

### Other

#### Registering Callback Triggers