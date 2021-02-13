
// *neode.onsave* setgo gcc -fsyntax-only gui.hpp 

#ifndef HAVE_GUI_H
#define HAVE_GUI_H

#include "pixelsurface.hpp"
#include <cstdint>
#include <string>
#include <vector>

/**
 * GuiEvent represents user-initiated events that arise in the user interface.
 */
class GuiEvent
{
public:
    /**
     * Event type of a keyboard button press event.
     */
    static const int KEYDOWN = 1;
    
    /**
     * Event type of a keyboard button release event.
     */
    static const int KEYUP = 2;
    
    /**
     * Event type of a mouse button press event.
     */
    static const int MOUSEDOWN = 3;
    
    /**
     * Event type of a mouse button release event.
     */
    static const int MOUSEUP = 4;
    
    /**
     * Event type of a mouse motion event.
     */
    static const int MOUSEMOVE = 5;
    
    /**
     * Create a bare event.
     * @param type Type of event
     */
    GuiEvent(int type) : type(type) { }
    
    /**
     * Create a keyboard button event.
     * @param type Type of event
     * @param button Keyboard button that was pressed or released
     */
    GuiEvent(int type, int button) : type(type), button(button) { }
    
    /**
     * Create a mouse (button or motion) event.
     * @param Type type of event
     * @param dx Amount of horizontal motion (if motion event)
     * @param dy Amount of vertical motion (if motion event)
     * @param button Button that was pressed (if button event)
     */
    GuiEvent(int type, int dx, int dy, int button) : type(type), dx(dx), dy(dy), button(button) { }
    
    /**
     * Type of event.
     */
    int type;
    
    /**
     * Amount of horizontal motion.
     */
    int dx;
    
    /**
     * Amount of vertical motion.
     */
    int dy;
    
    /**
     * Button pressed or released.
     */
    int button;
};


/**
 * Gui is an abstraction of a graphical user interface supporting graphical output and
 * some number of types of user input. The graphical output is exposed in the form of
 * a PixelSurface
 * @see PixelSurface
 */
class Gui : public PixelSurface
{
public:
    /**
     * Create a window.
     * @param title Window titlebar text
     * @param width Window width in pixels
     * @param height Window height in pixels
     */
    virtual void createWindow(const std::string title, int width, int height) = 0;
    
    /**
     * Get all pending events.
     * @param events Events are written to this list
     */
    virtual void getEvents(std::vector<GuiEvent>& events) = 0;
    
    /**
     * Clear the graphical output, painting it black.
     */
    virtual void clear() = 0;
    
    /**
     * Present the graphical output, updating the display with any changes made to the underlying
     * pixel array.
     */
    virtual void present() = 0;
    
    /**
     * Set the RGB color of the pixel located at (x, y)
     * @see PixelSurface
     */
    virtual void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    
    virtual ~Gui(){}
};

#endif
