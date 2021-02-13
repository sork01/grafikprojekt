
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only raytracingrenderer.hpp

#ifndef HAVE_RAYTRACINGRENDERER_H
#define HAVE_RAYTRACINGRENDERER_H

#include "renderer.hpp"
#include <pthread.h>

/**
 * RayTracingRenderer implements a Renderer that renders scenes using raytracing (no way!)
 */
class RayTracingRenderer : public Renderer
{
public:
    /**
     * Render a given scene by writing an image of the given width and height to the given output
     */
    void renderScene(Scene& s, int width, int height, PixelSurface& pixels);

private:
    /**
     * Thread data entry provided to each rendering thread
     */
    struct ThreadData
    {
        int thread_id;
        Scene* scene;
        PixelSurface* output;
        int width;
        int height;
        int x_from;
        int x_to;
        int y_from;
        int y_to;
        int* progress;
        pthread_mutex_t* progress_mutex;
    };
    
    /**
     * Pthreads thread entry point
     */
    static void* renderingThread(void* arg);
};

#endif
