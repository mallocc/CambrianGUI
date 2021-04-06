# ꞒambrianGUI
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
     	- Defining Event Callbacks
     	- Registering a Widget
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

## Setup

In order to program using *CambrianGUI*, you will need:
 - GLEW installed into you project
 - Niels Lohmann's [JSON C++ Header Library](https://github.com/nlohmann/json), headers go in your include directories
 - Hugh's [SplitTimer](https://github.com/thehugh100/Split-Timer) and [AssetPacker](https://github.com/thehugh100/Simple-Asset-Packer)
 - Windows API (planning to not need this in the future)
 - Thomas Monkman's [FileWatch](https://github.com/ThomasMonkman/filewatch)

## GUI Struture

### GUI

`GUI` is the main class that orchestrates the framework. 
This is where you feed input events and call you draw event.
It contains various managers that can be accessed by all your widget classes.
The mains ones we are interested in are `WidgetManager` and `Configuration`.
It is advised that your implementation inherits `GUI` and implements 
the virtual functions, though make sure to call the respective parent class methods
or your `GUI` can't function properly.

`virtual void init(std::string configOverridePath, bool firstLoad);` needs to be called in your initialisation stage of your program.

`virtual void draw();` should be called in your main graphic thread, as this uses GLEW API calls.

`virtual void onMouseEvent(MouseEventData mouseEventData);` should be called to pass mouse events to the GUI. 
`MouseEventData` is just a container for the mouse button flags.
Similarly `virtual void onKeyEvent(KeyEventData keyEventData);` should be called to pass key stroke events, where `KeyEventData` is the key button flags.

`GUI` is the bootstrap for passing information the widgets, so these are the main methods needed, as it should be all controlled through recursive `Widget` calls.

### WidgetManager

*todo*

### Configuration

*todo*

## Widgets

### Widget

`Widget` is the base class for defining a widget.
This stores widget properties such as position, size, event callbacks, etc.
`Widget` can be used as generic button be default, but would be advised to inherit and implement your own functions draw and callback logic, if nessecary.

`Widget* gui::Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus)` is the callback for mouse events.
This likely won't need to be reimplemented as it will call the approriate event callbacks that can be redefined.
The respective `callback_t` should be redefined if you want your widget to reactive to events and should be done in the contructor.
`virtual void draw(float tx, float ty, bool editMode = false);` is called recurisvely so be sure to make a call to your parent `Widget` classes.
`virtual bool init(nlohmann::json j, bool ignoreType = false);` should be reimplemented if you configuration for dervied class member properties. 
Examples should aid in how it should be implemented.

Widgets can have properties that change during runtime according to the configuration.
This is achieved by defining properties, and their data, which will be changed from another widgets event being triggered.
This causes the widget to effectivelly be reinitialised from configuration with these new properties.
Use `virtual void addToManifestList(nlohmann::json j, std::map<std::string, ManifestTuple> manifest);` in `init()` to link new properties to configuration.

### LayoutWidget

*todo*

## Event Flow

*CambrianGUI* uses an Delegate Event Model for event handling of different widgets. 
Each widget is its own event handler that uses a Event Bubbling event propagation to determine the intended widget in the DOM tree of elements.
This starts from the outermost element and recursively iterates the DOM tree.
When a widget in the tree decides that it should handle the event, it will consume it by returning `true`, so to signal to parents that is should stop searching.

e.g. 

`void GUI::onMouseEvent(MouseEventData mouseEventData);` 
call to the `GUI`'s root widget ->

`Widget* LayoutWidget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);`
root widget will most likely be a LayoutWidget, that has children widgets, which the event will be passed to ->
 
`Widget* Widget::onMouseEvent(MouseEventData mouseEventData, bool process, bool focus);` 
this is where the event parsed and if consumed, will be handled by the appropriate event callback.
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

## Examples
*todo*

### Implementation

#### Class Setup

#### Defining Event Callbacks

#### Registering a Widget

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