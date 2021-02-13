
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
    for (Object*const obj : s.getObjects())
    {
        obj->cullInvisibleParts(s.camera.getPosition());
    }
    
    // thread setup
    int NUM_THREADS = 8;
    pthread_t threads[NUM_THREADS];
    RayTracingRenderer::ThreadData thread_data[NUM_THREADS];
    
    pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;
    int progress = 0;
    
    int y_ofs = 0;
    
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_data[i].renderer = this;
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

list<Renderer::Option> RayTracingRenderer::getOptions()
{
    list<Option> options;
    
    options.push_back(Option("ShadowSamples", opt_ShadowSamples));
    
    return options;
}

void RayTracingRenderer::setOptions(list<Renderer::Option> opts)
{
    for (Option opt : opts)
    {
        setOption(opt);
    }
}

void RayTracingRenderer::setOption(Option opt)
{
    if (opt.name == "ShadowSamples")
    {
        opt_ShadowSamples = opt.intval;
    }
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
bool RayTracingRenderer::cast(RayTracingRenderer* renderer, const Scene* s, const list<Object*>& objects, const list<Light*>& lights, vec3 ray_org, vec3 ray_dir, vec3& color, int rayGeneration, bool enableCulling/*, bool debug*/)
{
    ray_dir = glm::normalize(ray_dir);
    
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
        
        if (obj->getIntersection(ray_org, ray_dir, is, enableCulling) && is.distance < closest.distance)
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
    
    if (closest.material.normalMap != nullptr)
    {
        vec3 nmapcolor = closest.material.normalMap->getColorAt(closest.textureCoords);
        vec3 nmapnormal = vec3(-1.0f + 2.0f*nmapcolor.x, -1.0f + 2.0f*nmapcolor.y, nmapcolor.z);
        
        vec3 vnorm = glm::normalize(closest.normal);
        vec3 u1 = glm::normalize(vec3(0, 1, -(vnorm.y / vnorm.z))); // TODO: proper handling of zero components
        
        // TODO: grok/explain
        vec3 u2 = glm::cross(vnorm, u1);
        mat3 nmapb(u2, u1, vnorm);
        
        /*if (debug)
        {
            printf("nmapcolor(%.2f, %.2f, %.2f)\n", nmapcolor.x, nmapcolor.y, nmapcolor.z);
            printf("nmapnormal(%.2f, %.2f, %.2f)\n", nmapnormal.x, nmapnormal.y, nmapnormal.z);
            printf("closest.normal(%.2f, %.2f, %.2f)\n", closest.normal.x, closest.normal.y, closest.normal.z);
            
            printf("vnorm(%.2f, %.2f, %.2f)\n", vnorm.x, vnorm.y, vnorm.z);
            printf("u1(%.2f, %.2f, %.2f)\n", u1.x, u1.y, u1.z);
            printf("u2(%.2f, %.2f, %.2f)\n", u2.x, u2.y, u2.z);
            
            vec3 tmp = nmapb * nmapnormal;
            
            printf("nmapb*nmapnormal(%.2f, %.2f, %.2f)\n", tmp.x, tmp.y, tmp.z);
        }*/
        
        closest.normal = nmapb * nmapnormal;
    }
    
    // perform Phong illumination
    color = ambientFactors * s->ambientLightPower;
    
    vec3 phong_N = glm::normalize(closest.normal);
    vec3 phong_V = glm::normalize(ray_org - closest.position);
    
    for (Light* const light : lights)
    {
        vec3 vr = light->position - closest.position;
        float r = sqrt(vr.x*vr.x + vr.y*vr.y + vr.z*vr.z);
        
        vec3 specPower = light->specularPower / (r*r);
        vec3 diffPower = light->diffusePower / (r*r);
        
        if (renderer->opt_ShadowSamples == 1)
        {
            Intersection shadow;
            
            for (Object* const obj : objects)
            {
                if (obj->getIntersection(closest.position + 0.1f*closest.normal, vr, shadow, false) && shadow.distance < r)
                {
                    specPower = diffPower = vec3(0,0,0);
                    break;
                }
            }
        }
        else if (renderer->opt_ShadowSamples > 1) // TODO: document/comment
        {
            vec3 vnorm = glm::normalize(vr);
            vec3 u1 = glm::normalize(vec3(0, 1, -(vnorm.y / vnorm.z))); // TODO: proper handling of zero components
            vec3 u2 = glm::cross(u1, vnorm);
            
            mat3 srb(u1, u2, vnorm);
            float phi = 0.015f;
            
            float theta = 0.0f;
            float theta_step = 4.0f*M_PI / float(renderer->opt_ShadowSamples);
            
            float light_amt = 1.0f;
            
            for (int j = 0; j < 2; ++j)
            {
                for (int i = 0; i < renderer->opt_ShadowSamples / 2; ++i)
                {
                    vec3 sr = srb * vec3(r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi));
                    
                    bool in_shadow = false;
                    Intersection shadow;
                    
                    for (Object* const obj : objects)
                    {
                        if (obj->getIntersection(closest.position + 0.1f*closest.normal, sr, shadow, false) && shadow.distance < r)
                        {
                            in_shadow = true;
                            break;
                        }
                    }
                    
                    theta += theta_step;
                    
                    if (in_shadow)
                    {
                        light_amt -= 1.0f / float(renderer->opt_ShadowSamples);
                        continue;
                    }
                }
                
                phi += phi;
            }
            
            specPower *= light_amt;
            diffPower *= light_amt;
        }
        
        vec3 phong_Lm = glm::normalize(light->position - closest.position);
        vec3 phong_Rm = 2.0f*(glm::dot(phong_Lm, phong_N))*phong_N - phong_Lm;
        
        color += glm::max(0.0f, glm::dot(phong_Lm, phong_N)) * (diffuseFactors * diffPower);
        
        if (closest.material.shininess > 0.0f)
        {
            color += pow(glm::max(0.0f, glm::dot(phong_Rm, phong_V)), closest.material.shininess) * (specularFactors * specPower);
        }
    }
    
    // opacity
    if (closest.material.opacity != vec3(1,1,1))
    {
        vec3 color_behind(0,0,0);
        
        if (closest.material.refraction != 1.0f)
        {
            // compute direction of refracted ray
            // vec3 T;
            // float n1, n2, theta1, theta2, costhet1, n1overn2, costheta2;
            // n2 = closest.material.refraction;
            // vec3 N = glm::normalize(closest.normal);
            // vec3 I = glm::normalize(ray_dir);
            // n1 = 1.0f;
            // theta1 = acos(glm::dot(I, N));
            // theta2 = asin(n1/n2)*(sqrt(1-pow(cos(theta1),2)));
            // costhet1 = cos(theta1);
            // n1overn2 = n1/n2;
            // costheta2 = cos(theta2);
            // T = (n1overn2*I+costhet1*(n1overn2)-(costheta2*N));
            // cast(renderer, s, objects, lights, closest.position + 0.001f * ray_dir, ray_dir * T, color_behind, rayGeneration + 1, false);
            
            // source: https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
            // {
            vec3 I = ray_dir;
            vec3 N = glm::normalize(closest.normal);
            
            float cosi = glm::clamp(glm::dot(I, N), -1.0f, 1.0f);
            float etai = 1;
            float etat = closest.material.refraction;
            
            vec3 n = N;
            
            if (cosi < 0)
            {
                cosi = -cosi;
            }
            else
            {
                std::swap(etai, etat);
                n = -N;
            }
            
            float eta = etai / etat;
            float k = 1 - eta * eta * (1 - cosi * cosi);
            
            // return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
            
            if (k > 0)
            {
                vec3 T = eta * I + (eta * cosi - sqrtf(k)) * n;
                cast(renderer, s, objects, lights, closest.position + 0.001f * T, T, color_behind, rayGeneration + 1, false/*, debug*/);
            }
            // }
        }
        else
        {
            // cast ray in same direction as if having passed straight thru the material
            cast(renderer, s, objects, lights, closest.position + 0.001f * ray_dir, ray_dir, color_behind, rayGeneration + 1, enableCulling/*, debug*/);
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
        cast(renderer, s, objects, lights, closest.position + 0.001f*re_reflect, re_reflect, color_reflect, rayGeneration + 1, false/*, debug*/);
        
        if (closest.material.metallic)
        {
            color_reflect = glm::normalize(closest.material.diffuseFactors)*vec3((color_reflect.x + color_reflect.y + color_reflect.z) / 3.0f);
        }
        
        if (closest.material.reflectionMap != nullptr)
        {
            color += color_reflect*closest.material.reflectivity*closest.material.reflectionMap->getColorAt(closest.textureCoords)*REFLECTION_ATTENUATE;
        }
        else
        {
            color += color_reflect*closest.material.reflectivity*REFLECTION_ATTENUATE;
        }
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
    
    // bool dodebug = false;
    
    // process only our part of the screen
    for (int y = data.y_from; y < data.y_to; y += 1)
    {
        for (int x = data.x_from; x < data.x_to; x += 1)
        {
            vec3 ray = s->camera.getRotationMatrix() * vec3(x - data.width/2, y - data.height/2, F);
            vec3 color;
            
            /*if (x == 200 && y == 100)
            {
                printf("dodebug\n");
                dodebug = true;
            }
            else
            {
                dodebug = false;
            }*/
            
            if (cast(data.renderer, s, objects, lights, campos, ray, color, 0, true/*, dodebug*/))
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
        
        fflush(stdout);
        
        pthread_mutex_unlock(data.progress_mutex);
    }
    
    pthread_exit(NULL);
}
