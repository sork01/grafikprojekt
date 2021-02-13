
// *neode.onsave* setgo gcc -I/usr/include/SDL2 -fsyntax-only sdlgui.hpp 

#ifndef HAVE_SDLGUI_H
#define HAVE_SDLGUI_H

#include "gui.hpp"
#include "SDL.h"
#include <string>
#include <vector>

using namespace std;

/**
 * Implementation of the Gui interface (and then some) using SDL2
 */
class SDLGui : public Gui
{
public:
    /**
     * Initialize SDL
     */
    SDLGui();
    
    /**
     * Create a window with a given title, width and height
     */
    void createWindow(const string title, int width, int height);
    
    /**
     * Get all pending events.
     */
    void getEvents(vector<GuiEvent>& events);
    
    /**
     * Clear the graphical output, painting it black.
     */
    void clear();
    
    /**
     * Present the graphical output, updating the display with any changes made to the underlying
     * pixel array.
     */
    void present();
    
    /**
     * Set the RGB color of the pixel located at (x, y)
     */
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    ~SDLGui();
    
    /**
     * Save a screenshot of the current display buffer
     */
    void saveBMP(string filename);

private:
    /**
     * Width of the graphical output
     */
    int width;
    
    /**
     * Height of the graphical output
     */
    int height;
    
    /**
     * Main window
     */
    SDL_Window* window = nullptr;
    
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    
    /**
     * Underlying pixel array
     */
    uint32_t* pixels32 = nullptr;
};

#endif
