
// *neode.onsave* setgo gcc -I/usr/include/glm -fsyntax-only raytracingrenderer.cpp

#include "raytracingrenderer.hpp"
#include <iostream>
#include <pthread.h>

#define REFLECTION_ATTENUATE 0.9f

/**
 * Render a given scene by writing an image of the given width and height to the given output
 */
void RayTracingRenderer::renderScene(Scene& s, int width, int height, PixelSurface& output)
{
    // ask for backface culling
    // for (Object*const obj : s.getObjects())
    // {
        // obj->cullInvisibleParts(s.camera.getPosition());
    // }
    
    // thread setup
    int NUM_THREADS = 8;
    pthread_t threads[NUM_THREADS];
    RayTracingRenderer::ThreadData thread_data[NUM_THREADS];
    
    pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;
    int progress = 0;
    
    int y_ofs = 0;
    
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_data[i].thread_id = i;
        thread_data[i].scene = &s;
        thread_data[i].output = &output;
        thread_data[i].width = width;
        thread_data[i].height = height;
        
        // each thread processes the entire width of the screen
        thread_data[i].x_from = 0;
        thread_data[i].x_to = width;
        
        // each thread processes its own part of the height of the screen
        thread_data[i].y_from = y_ofs;
        thread_data[i].y_to = y_ofs + height / NUM_THREADS;
        
        thread_data[i].progress = &progress;
        thread_data[i].progress_mutex = &progress_mutex;
        
        y_ofs += height / NUM_THREADS;
        
        pthread_create(&threads[i], NULL, RayTracingRenderer::renderingThread, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&progress_mutex);
}

/**
 * Is any component != 0 ?
 */
inline bool any(vec3 v)
{
    return v.x != 0 || v.y != 0 || v.z != 0;
}

/**
 * Raycasting function. Returns True if ray intersected with object and writes resulting color
 * to given parameter, return False otherwise.
 */
bool cast(const Scene* s, const list<Object*>& objects, const list<Light*>& lights, vec3 ray_org, vec3 ray_dir, vec3& color, int rayGeneration)
{
    // prevent infinite loops of reflection or refraction
    if (rayGeneration > 4)
    {
        return false;
    }
    
    bool found = false;
    Intersection closest;
    closest.distance = std::numeric_limits<float>::max();
    
    // find the closest intersection
    for (Object* const obj : objects)
    {
        Intersection is;
        
        if (obj->getIntersection(ray_org, ray_dir, is) && is.distance < closest.distance)
        {
            closest = is;
            found = true;
        }
    }
    
    if (!found)
    {
        return false;
    }
    
    // start color processing
    vec3 specularFactors = closest.material.specularFactors;
    vec3 diffuseFactors = closest.material.diffuseFactors;
    vec3 ambientFactors = closest.material.ambientFactors;
    
    // TODO: is it worth checking if two or more texture pointers are identical and avoid
    //       calling repeated calls to getColorAt?
    
    // apply textures if any
    if (closest.material.specularTexture != nullptr)
    {
        specularFactors *= closest.material.specularTexture->getColorAt(closest.textureCoords);
    }
    
    if (closest.material.diffuseTexture != nullptr)
    {
        diffuseFactors *= closest.material.diffuseTexture->getColorAt(closest.textureCoords);
    }
    
    if (closest.material.ambientTexture != nullptr)
    {
        ambientFactors *= closest.material.ambientTexture->getColorAt(closest.textureCoords);
    }
    
    // perform Phong illumination
    color = ambientFactors * s->ambientLightPower;
    
    vec3 phong_N = glm::normalize(closest.normal);
    vec3 phong_V = glm::normalize(ray_org - closest.position);
    
    for (Light* const light : lights)
    {
        vec3 vr = light->position - closest.position;
        float r = sqrt(vr.x*vr.x + vr.y*vr.y + vr.z*vr.z);
        
        // soft shadow perturbations
        vec3 vsr[12];
        float shadpart = 1.0f/12.0f;
        float light_amt = 1.0f;
        
        vsr[0] = vr + vec3(-0.08f, 0, 0);
        vsr[1] = vr + vec3(-0.04f, 0, 0);
        vsr[4] = vr + vec3( 0.04f, 0, 0);
        vsr[3] = vr + vec3( 0.08f, 0, 0);
        vsr[8] = vr + vec3(0, -0.08f, 0);
        vsr[5] = vr + vec3(0, -0.04f, 0);
        vsr[6] = vr + vec3(0, 0.04f, 0);
        vsr[7] = vr + vec3(0, 0.08f, 0);
        vsr[8] = vr + vec3(0, 0, -0.08f);
        vsr[9] = vr + vec3(0, 0, -0.04f);
        vsr[10] = vr + vec3(0, 0, 0.04f);
        vsr[11] = vr + vec3(0, 0, 0.08f);
        
        // shadow tests
        for (Object* const obj : objects)
        {
            Intersection shadow;
            
            for (int j = 0; j < 12; ++j)
            {
                if (obj->getIntersection(closest.position + 0.0001f * vsr[j], vsr[j], shadow) && shadow.distance < r)
                {
                    light_amt -= shadpart;
                }
            }
        }
        
        vec3 specPower = light_amt * light->specularPower / (r*r);
        vec3 diffPower = light_amt * light->diffusePower / (r*r);
        
        vec3 phong_Lm = glm::normalize(light->position - closest.position);
        vec3 phong_Rm = 2.0f*(glm::dot(phong_Lm, phong_N))*phong_N - phong_Lm;
        
        color += glm::max(0.0f, glm::dot(phong_Lm, phong_N)) * (diffuseFactors * diffPower);
        
        if (closest.material.shininess > 0.0f)
        {
            color += pow(glm::max(0.0f, glm::dot(phong_Rm, phong_V)), closest.material.shininess) * (specularFactors * specPower);
        }
    }
    
    // opacity
    if (any(closest.material.opacity))
    {
        vec3 color_behind(0,0,0);
        
        if (closest.material.refraction != 1.0f)
        {
            // compute direction of refracted ray
            vec3 T;
            float n1, n2, theta1, theta2, costhet1, n1overn2, costheta2;
            n2 = closest.material.refraction;
            vec3 N = glm::normalize(closest.normal);
            vec3 I = glm::normalize(ray_dir);
            n1 = 1.0f;
            theta1 = acos(glm::dot(I, N));
            theta2 = asin(n1/n2)*(sqrt(1-pow(cos(theta1),2)));
            costhet1 = cos(theta1);
            n1overn2 = n1/n2;
            costheta2 = cos(theta2);
            T = (n1overn2*I+costhet1*(n1overn2)-(costheta2*N));
            cast(s, objects, lights, closest.position + 0.0001f * ray_dir,  ray_dir * T, color_behind, rayGeneration + 1);       
        }
        else
        {
            // cast ray in same direction as if having passed straight thru the material
            cast(s, objects, lights, closest.position + 0.0001f * ray_dir, ray_dir, color_behind, rayGeneration + 1);      
        }
        // TODO: proper blending?
        
        color = color_behind*(vec3(1.0f)-closest.material.opacity) + color*closest.material.opacity;
    }
    
    // reflection
    if (any(closest.material.reflectivity))
    {
        vec3 color_reflect(0,0,0);
        
        vec3 V = glm::normalize(ray_dir);
        vec3 N = glm::normalize(closest.normal);
        
        // compute direction of reflected ray and cast it
        vec3 re_reflect = V - 2.0f*(glm::dot(V, N))*N;
        cast(s, objects, lights, closest.position + 0.0001f*re_reflect, re_reflect, color_reflect, rayGeneration + 1);
        
        if (closest.material.metallic)
        {
            color_reflect = glm::normalize(closest.material.diffuseFactors)*vec3((color_reflect.x + color_reflect.y + color_reflect.z) / 3.0f);
        }
        
        color += color_reflect*closest.material.reflectivity*REFLECTION_ATTENUATE;
    }
    
    return true;
}

void* RayTracingRenderer::renderingThread(void* arg)
{
    RayTracingRenderer::ThreadData data = (*(ThreadData*) arg);
    
    // cout << "starting thread " << data.thread_id << endl;
    
    Scene* s = data.scene;
    
    const list<Object*> objects = s->getObjects();
    const list<Light*> lights = s->getLights();
    
    float F = s->camera.getFocalLength();
    vec3 campos = s->camera.getPosition();
    
    // process only our part of the screen
    for (int y = data.y_from; y < data.y_to; y += 1)
    {
        for (int x = data.x_from; x < data.x_to; x += 1)
        {
            vec3 ray = s->camera.getRotationMatrix() * vec3(x - data.width/2, y - data.height/2, F);
            vec3 color;
            
            if (cast(s, objects, lights, campos, ray, color, 0))
            {
                data.output->setPixel(x,y,
                    glm::clamp(color.x * 255.0f, 0.0f, 255.0f),
                    glm::clamp(color.y * 255.0f, 0.0f, 255.0f),
                    glm::clamp(color.z * 255.0f, 0.0f, 255.0f));
            }
        }
        
        // print progress counter
        pthread_mutex_lock(data.progress_mutex);
        *data.progress = *data.progress + 1;
        
        cout << "progress: " << *data.progress << "/" << data.height << "\r";
        
        if (*data.progress == data.height)
        {
            cout << endl;
        }
        
        pthread_mutex_unlock(data.progress_mutex);
    }
    
    pthread_exit(NULL);
}
