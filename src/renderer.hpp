
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only renderer.hpp

#ifndef HAVE_RENDERER_H
#define HAVE_RENDERER_H

#include "pixelsurface.hpp"
#include "scene.hpp"
#include <cstdint>
#include <list>
#include <string>

/**
 * Renderer is an abstract class representing a system capable of producing
 * a two-dimensional image.
 */
class Renderer
{
public:
    enum OptionType {String, Bool, Integer, Float};
    
    struct Option
    {
        Option(string name, string strval) : type(String), name(name), strval(strval) {}
        Option(string name, bool boolval) : type(Bool), name(name), boolval(boolval) {}
        Option(string name, int intval) : type(Integer), name(name), intval(intval) {}
        Option(string name, float floatval) : type(Float), name(name), floatval(floatval) {}
        
        OptionType type;
        string name;
        
        string strval = "";
        bool boolval = false;
        int intval = 0;
        float floatval = 0.0f;
    };
    
    /**
     * Render a scene.
     * @param s Scene to render
     * @param width Width of output image (TODO: grab these from PixelSurface.getDimensions?)
     * @param height Height of output image
     * @param pixels Pixel surface for output
     */
    virtual void renderScene(Scene& s, int width, int height, PixelSurface& output) = 0;
    virtual list<Option> getOptions() = 0;
    virtual void setOptions(list<Option> options) = 0;
    virtual void setOption(Option option) = 0;
    virtual ~Renderer(){}
};

#endif
