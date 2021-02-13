
// *neode.onsave* setgo gcc -I/usr/include/SDL2 -fsyntax-only sdlgui.cpp 

#include "sdlgui.hpp"

using namespace std;

/**
 * Initialize SDL
 */
SDLGui::SDLGui()
{
    SDL_Init(SDL_INIT_VIDEO);
}

/**
 * Create a window with a given title, width and height
 */
void SDLGui::createWindow(const string title, int width, int height)
{
    this->width = width;
    this->height = height;
    
    window = SDL_CreateWindow(title.c_str(),
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height, /*SDL_WINDOW_RESIZABLE*/0);
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, width, height);
    
    pixels32 = (uint32_t*) malloc(width * height * sizeof(uint32_t));
}

/**
 * Get all pending events.
 */
void SDLGui::getEvents(vector<GuiEvent>& events)
{
    SDL_Event ev;
    
    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
            case SDL_KEYDOWN:
                events.push_back(GuiEvent(GuiEvent::KEYDOWN, ev.key.keysym.scancode));
                break;
            
            case SDL_KEYUP:
                events.push_back(GuiEvent(GuiEvent::KEYUP, ev.key.keysym.scancode));
                break;
            
            default:
                break;
        }
    }
}

/**
 * Clear the graphical output, painting it black.
 */
void SDLGui::clear()
{
    memset(pixels32, 0, width * height * sizeof(uint32_t));
}

/**
 * Present the graphical output, updating the display with any changes made to the underlying
 * pixel array.
 */
void SDLGui::present()
{
    // such sdl2, much wow
    SDL_UpdateTexture(texture, NULL, pixels32, width * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

/**
 * Set the RGB color of the pixel located at (x, y)
 */
void SDLGui::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }
    
    int ofs = y * width + x;
    
    pixels32[ofs] = (r << 16) | (g << 8) | b;
}

/**
 * Cleanup and SDL exit
 */
SDLGui::~SDLGui()
{
    if (window != nullptr)
    {
        free(pixels32);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    
    SDL_Quit();
}

/**
 * Save a screenshot of the current display buffer
 * Source: https://stackoverflow.com/a/22339011
 */
void SDLGui::saveBMP(string filename)
{
    SDL_Surface* sshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot, filename.c_str());
    SDL_FreeSurface(sshot);
}
