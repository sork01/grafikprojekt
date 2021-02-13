
// *neode.onsave* setgo gcc -pthread -I/usr/include/lua5.3 -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glm -I/usr/include/SDL2 -fsyntax-only main.cpp 

// todo:
    // lua stuff in separate file
    // lua render abort
    // fix stl lua loading
    // Object::move, rotate, flipXYZ, flipnormals? scale, implement in mesh and sphere, add lua access
    // lua mesh vertex modification access somehow

#include "lua.hpp"
#include "objreader.hpp"
#include "stlreader.hpp"
#include "usbscancodes.hpp"
#include "filedialog.hpp"
#include "gtkfiledialog.hpp"
#include "mesh.hpp"
#include "sphere.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "wireframerenderer.hpp"
#include "raytracingrenderer.hpp"
#include "sdlgui.hpp"
#include "sdltexture.hpp"
#include "SDL.h"
#include <sys/time.h>
#include <cstdio>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

FileDialog::Response runDialog(FileDialog& dialog, string& filename, string& ext);
Scene scene;
STLReader readerstl;
OBJReader readerobj;
SDLGui gui;

uint single_rayrender_state = 0;

Renderer* ray;
Renderer* wire;
Renderer* renderer;

Light* movableLight;

struct Controls
{
    bool left, right, forward, back, up, down, turnleft, turnright, lookup, lookdown;
};

vector<Material> lua_materials;
vector<Light*> lua_lights;
vector<Texture*> lua_textures;
vector<Mesh*> lua_meshes;
vector<Sphere*> lua_spheres;

Mesh* lua_current_mesh;
vec3 lua_vpos;
vec3 lua_vnormal;
vec2 lua_vtexcoords;
string luaname = "script.lua";

static int lua_load_texture(lua_State* L);
static int lua_print(lua_State* L);
static int lua_scene_clear(lua_State* L);
static int lua_scene_set_ambient_light(lua_State* L);
static int lua_camera_set_pos(lua_State* L);
static int lua_camera_set_focal_length(lua_State* L);
static int lua_camera_set_pitch(lua_State* L);
static int lua_camera_set_yaw(lua_State* L);
static int lua_add_light(lua_State* L);
static int lua_copy_light(lua_State* L);
static int lua_light_set_pos(lua_State* L);
static int lua_light_set_diffuse(lua_State* L);
static int lua_light_set_specular(lua_State* L);
static int lua_add_material(lua_State* L);
static int lua_copy_material(lua_State* L);
static int lua_material_set_ambient(lua_State* L);
static int lua_material_set_diffuse(lua_State* L);
static int lua_material_set_specular(lua_State* L);
static int lua_material_set_ambient_texture(lua_State* L);
static int lua_material_set_diffuse_texture(lua_State* L);
static int lua_material_set_specular_texture(lua_State* L);
static int lua_material_set_normal_map(lua_State* L);
static int lua_material_set_reflection_map(lua_State* L);
static int lua_material_set_opacity(lua_State* L);
static int lua_material_set_refraction(lua_State* L);
static int lua_material_set_reflectivity(lua_State* L);
static int lua_material_set_metallic(lua_State* L);
static int lua_material_set_shininess(lua_State* L);
static int lua_add_sphere(lua_State* L);
static int lua_sphere_set_pos(lua_State* L);
static int lua_sphere_set_radius(lua_State* L);
static int lua_sphere_move(lua_State* L);
static int lua_sphere_rotate(lua_State* L);
static int lua_sphere_scale(lua_State* L);
static int lua_add_mesh(lua_State* L);
static int lua_vertex_pos(lua_State* L);
static int lua_vertex_normal(lua_State* L);
static int lua_vertex_texture_coords(lua_State* L);
static int lua_mesh_add_vertex(lua_State* L);
static int lua_mesh_commit_triangle(lua_State* L);
static int lua_mesh_finalize(lua_State* L);
static int lua_mesh_flip_x(lua_State* L);
static int lua_mesh_flip_y(lua_State* L);
static int lua_mesh_flip_z(lua_State* L);
static int lua_mesh_set_pos(lua_State* L);
static int lua_mesh_move(lua_State* L);
static int lua_mesh_rotate(lua_State* L);
static int lua_mesh_scale(lua_State* L);
static int lua_mesh_apply_material(lua_State* L);
static int lua_load_obj(lua_State* L);
static int lua_load_stl(lua_State* L);
static int lua_render(lua_State* L);
static int lua_savebmp(lua_State* L);
static int lua_option(lua_State* L);

void loadlua(lua_State* L);

int main(int argc, char** argv)
{
    lua_State* L = nullptr;
    
    Controls controls;
    controls.left = controls.right = controls.forward = controls.back = controls.up = controls.down = false;
    controls.turnleft = controls.turnright = controls.lookup = controls.lookdown = false;
    
    gui.createWindow("Test", 640, 480);
    
    scene.ambientLightPower = vec3(1.0f);
    scene.camera.setPosition(0, 0, -3.001);
    
    movableLight = new Light();
    movableLight->position = vec3(0, 0, -0.8f);
    movableLight->specularPower = vec3(10.0f);
    movableLight->diffusePower = vec3(10.0f);
    scene.addLight(movableLight);
    
    ray = new RayTracingRenderer();
    wire = new WireframeRenderer();
    
    wire->setOption(Renderer::Option("ShowVertexNormals", false));
    wire->setOption(Renderer::Option("ShowCentroidNormals", false));
    
    // for toggling display of normals
    int wireoptions = 0;
    
    ray->setOption(Renderer::Option("ShadowSamples", 0));
    
    // for toggling between different number of shadow samples
    int shadowsetting = 0;
    
    renderer = wire;
    
    bool quit = false;
    
    // handle command line arguments
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            int len = strnlen(argv[i], 256);
            
            if (strncmp(argv[i] + len - 4, ".lua", 4) == 0)
            {
                luaname = string(argv[i]);
                loadlua(L);
            }
        }
    }
    
    while (!SDL_QuitRequested() && !quit)
    {
        vector<GuiEvent> events;
        gui.getEvents(events); // if renaming gui to graphics, input doesn't fit in there anymore
        
        for (const GuiEvent& ev : events)
        {
            switch (ev.type)
            {
                case GuiEvent::KEYDOWN:
                    switch (ev.button)
                    {
                        case USBScanCodes::W:
                            controls.forward = true;
                            break;
                        
                        case USBScanCodes::A:
                            controls.left = true;
                            break;
                        
                        case USBScanCodes::S:
                            controls.back = true;
                            break;
                        
                        case USBScanCodes::D:
                            controls.right = true;
                            break;
                        
                        case USBScanCodes::Digit2:
                            controls.up = true;
                            break;
                        
                        case USBScanCodes::X:
                            controls.down = true;
                            break;
                        
                        case USBScanCodes::LeftArrow:
                            controls.turnleft = true;
                            break;
                        
                        case USBScanCodes::RightArrow:
                            controls.turnright = true;
                            break;
                        
                        case USBScanCodes::UpArrow:
                            controls.lookup = true;
                            break;
                        
                        case USBScanCodes::DownArrow:
                            controls.lookdown = true;
                            break;
                        
                        case USBScanCodes::F:
                        {
                            GtkFileDialog dialog;
                            
                            string filename;
                            string fileext;
                            
                            if (runDialog(dialog, filename, fileext) == FileDialog::Response::Accept)
                            {
                                Mesh* mesh;
                                
                                if (fileext == "stl" || fileext == "obj")
                                {
                                    if (fileext == "stl")
                                    {
                                        mesh = readerstl.loadStl(filename);
                                    }
                                    else
                                    {
                                        mesh = readerobj.loadFile(filename);
                                    }
                                    
                                    mesh->flipY();
                                    mesh->colorize();
                                    mesh->computeBVH();
                                    scene.addObject(mesh);
                                }
                                else if (fileext == "lua")
                                {
                                    luaname = filename;
                                    loadlua(L);                                    
                                }
                                else
                                {
                                    cout << "unsupported file format" << endl;
                                }
                            }
                            
                            break;
                        }
                        
                        case USBScanCodes::T:
                        {
                            if (single_rayrender_state == 0)
                            {
                                renderer = ray;
                                single_rayrender_state = 1;
                            }
                            else if (single_rayrender_state == 2)
                            {
                                renderer = wire;
                                single_rayrender_state = 0;
                            }
                            
                            break;
                        }
                        
                        case USBScanCodes::Q:
                        {
                            cout << "Q pressed, model(s) removed" << endl;
                            
                            for (Object* o : scene.getObjects())
                            {
                                delete o;
                                scene.removeObject(o);
                            }
                            
                            break;
                        }
                        
                        case USBScanCodes::R:
                            if (renderer == wire)
                            {
                                renderer = ray;
                            }
                            else
                            {
                                renderer = wire;
                            }
                            
                            break;
                        
                        case USBScanCodes::Y:
                            cout << "light forward" << endl;
                            movableLight->position += vec3(0,0,0.1f);
                            break;
                        case USBScanCodes::I:
                            cout << "light backward" << endl;
                            movableLight->position += vec3(0,0,-0.1f);
                            break;
                        case USBScanCodes::U:
                            cout << "light up" << endl;
                            movableLight->position += vec3(0,-0.1f,0);
                            break;
                        case USBScanCodes::N:
                            cout << "light down" << endl;
                            movableLight->position += vec3(0,0.1f,0);
                            break;
                        case USBScanCodes::H:
                            cout << "light left" << endl;
                            movableLight->position += vec3(-0.1f,0,0);
                            break;
                        case USBScanCodes::J:
                            cout << "light right" << endl;
                            movableLight->position += vec3(0.1f,0,0);
                            break;
                        
                        case USBScanCodes::F5:
                            loadlua(L);
                            break;
                        
                        case USBScanCodes::F9:
                            switch (wireoptions)
                            {
                                case 0:
                                    wire->setOption(Renderer::Option("ShowVertexNormals", true));
                                    wire->setOption(Renderer::Option("ShowCentroidNormals", false));
                                    wireoptions = 1;
                                    break;
                                
                                case 1:
                                    wire->setOption(Renderer::Option("ShowVertexNormals", false));
                                    wire->setOption(Renderer::Option("ShowCentroidNormals", true));
                                    wireoptions = 2;
                                    break;
                                
                                case 2:
                                    wire->setOption(Renderer::Option("ShowVertexNormals", true));
                                    wire->setOption(Renderer::Option("ShowCentroidNormals", true));
                                    wireoptions = 3;
                                    break;
                                
                                case 3:
                                    wire->setOption(Renderer::Option("ShowVertexNormals", false));
                                    wire->setOption(Renderer::Option("ShowCentroidNormals", false));
                                    wireoptions = 0;
                                    break;
                                
                                default:
                                    break;
                            }
                            break;
                        
                        case USBScanCodes::F10:
                            switch (shadowsetting)
                            {
                                case 0:
                                    ray->setOption(Renderer::Option("ShadowSamples", 1));
                                    cout << "shadow samples: 1" << endl;
                                    ++shadowsetting;
                                    break;
                                
                                case 1:
                                    ray->setOption(Renderer::Option("ShadowSamples", 16));
                                    cout << "shadow samples: 16" << endl;
                                    ++shadowsetting;
                                    break;
                                
                                default:
                                    ray->setOption(Renderer::Option("ShadowSamples", 0));
                                    cout << "shadows disabled" << endl;
                                    shadowsetting = 0;
                                    break;
                            }
                            break;
                        
                        case USBScanCodes::F11:
                        {
                            vec3 campos = scene.camera.getPosition();
                            float camyaw = scene.camera.getYaw();
                            float campitch = scene.camera.getPitch();
                            
                            printf("camera: pos(%.2f, %.2f, %.2f), yaw=%.2f, pitch=%.2f\n", campos.x, campos.y, campos.z, camyaw, campitch);
                            break;
                        }
                        
                        case USBScanCodes::Escape:
                            quit = true;
                            break;
                        
                        default:
                            break;
                    }
                    
                    break;
                
                case GuiEvent::KEYUP:
                    switch (ev.button)
                    {
                        case USBScanCodes::W:
                            controls.forward = false;
                            break;
                        
                        case USBScanCodes::A:
                            controls.left = false;
                            break;
                        
                        case USBScanCodes::S:
                            controls.back = false;
                            break;
                        
                        case USBScanCodes::D:
                            controls.right = false;
                            break;
                        
                        case USBScanCodes::Digit2:
                            controls.up = false;
                            break;
                        
                        case USBScanCodes::X:
                            controls.down = false;
                            break;
                        
                        case USBScanCodes::LeftArrow:
                            controls.turnleft = false;
                            break;
                        
                        case USBScanCodes::RightArrow:
                            controls.turnright = false;
                            break;
                        
                        case USBScanCodes::UpArrow:
                            controls.lookup = false;
                            break;
                        
                        case USBScanCodes::DownArrow:
                            controls.lookdown = false;
                            break;
                        
                        default:
                            break;
                    }
                    break;
                
                default:
                    break;
            }
        }
        
        if (controls.left)
        {
            scene.camera.moveLeft(0.1f);
        }
        if (controls.right)
        {
            scene.camera.moveRight(0.1f);
        }
        if (controls.forward)
        {
            scene.camera.moveForward(0.1f);
        }
        if (controls.back)
        {
            scene.camera.moveBack(0.1f);
        }
        if (controls.up)
        {
            scene.camera.moveUp(0.1f);
        }
        if (controls.down)
        {
            scene.camera.moveDown(0.1f);
        }
        
        if (controls.turnleft)
        {
            scene.camera.turnLeft(0.04f);
        }
        if (controls.turnright)
        {
            scene.camera.turnRight(0.04f);
        }
        if (controls.lookup)
        {
            scene.camera.pitchUp(0.04f);
        }
        if (controls.lookdown)
        {
            scene.camera.pitchDown(0.04f);
        }
        
        if (single_rayrender_state != 2)
        {
            gui.clear();
            
            struct timeval tv1;
            struct timeval tv2;
            struct timezone tz;
            
            double time1 = 0.0;
            double time2 = 0.0;
            
            gettimeofday(&tv1, &tz);
            time1 = double(tv1.tv_sec) + double(tv1.tv_usec) / 1000000.0;
            
            renderer->renderScene(scene, 640, 480, gui);
            gui.present();
            
            if (single_rayrender_state == 1)
            {
                single_rayrender_state = 2;
            }
            
            gettimeofday(&tv2, &tz);
            time2 = double(tv2.tv_sec) + double(tv2.tv_usec) / 1000000.0;
            
            if (renderer == ray)
            {
                printf("render time: %.1f ms  (%.1f potential fps)\n", (time2 - time1)*1000.0, 1000.0/((time2 - time1)*1000.0));
            }
        }
        else
        {
            gui.present();
        }
        
        SDL_Delay(16);
    }
    
    delete renderer;
    
    return 0;
}

FileDialog::Response runDialog(FileDialog& dialog, string& filename, string& ext)
{
    dialog.show();
    
    if (dialog.getResponse() == FileDialog::Response::Accept && dialog.getFilename(filename))
    {
        ext = filename.substr(filename.size() - 3);
    }
    
    return dialog.getResponse();
}

void loadlua(lua_State* L)
{
    if (single_rayrender_state == 2)
    {
        single_rayrender_state = 1;
    }
    
    if (L != nullptr)
    {
        lua_close(L);
    }
    
    L = luaL_newstate();
    
    luaL_openlibs(L);
    lua_register(L, "print", &lua_print);
    lua_register(L, "scene_clear", &lua_scene_clear);
    lua_register(L, "scene_set_ambient_light", &lua_scene_set_ambient_light);
    lua_register(L, "camera_set_pos", &lua_camera_set_pos);
    lua_register(L, "camera_set_focal_length", &lua_camera_set_focal_length);
    lua_register(L, "camera_set_pitch", &lua_camera_set_pitch);
    lua_register(L, "camera_set_yaw", &lua_camera_set_yaw);
    lua_register(L, "load_stl", &lua_load_stl);
    lua_register(L, "load_obj", &lua_load_obj);
    lua_register(L, "add_light", &lua_add_light);
    lua_register(L, "copy_light", &lua_copy_light);
    lua_register(L, "light_set_pos", &lua_light_set_pos);
    lua_register(L, "light_set_diffuse", &lua_light_set_diffuse);
    lua_register(L, "light_set_specular", &lua_light_set_specular);
    lua_register(L, "add_material", &lua_add_material);
    lua_register(L, "copy_material", &lua_copy_material);
    lua_register(L, "load_texture", &lua_load_texture);
    lua_register(L, "material_set_ambient", &lua_material_set_ambient);
    lua_register(L, "material_set_diffuse", &lua_material_set_diffuse);
    lua_register(L, "material_set_specular", &lua_material_set_specular);
    lua_register(L, "material_set_ambient_texture", &lua_material_set_ambient_texture);
    lua_register(L, "material_set_diffuse_texture", &lua_material_set_diffuse_texture);
    lua_register(L, "material_set_specular_texture", &lua_material_set_specular_texture);
    lua_register(L, "material_set_normal_map", &lua_material_set_normal_map);
    lua_register(L, "material_set_reflection_map", &lua_material_set_reflection_map);
    lua_register(L, "material_set_opacity", &lua_material_set_opacity);
    lua_register(L, "material_set_refraction", &lua_material_set_refraction);
    lua_register(L, "material_set_reflectivity", &lua_material_set_reflectivity);
    lua_register(L, "material_set_metallic", &lua_material_set_metallic);
    lua_register(L, "material_set_shininess", &lua_material_set_shininess);
    lua_register(L, "add_sphere", &lua_add_sphere);
    lua_register(L, "sphere_set_pos", &lua_sphere_set_pos);
    lua_register(L, "sphere_set_radius", &lua_sphere_set_radius);
    lua_register(L, "sphere_move", &lua_sphere_move);
    lua_register(L, "sphere_rotate", &lua_sphere_rotate);
    lua_register(L, "sphere_scale", &lua_sphere_scale);
    lua_register(L, "add_mesh", &lua_add_mesh);
    lua_register(L, "vertex_pos", &lua_vertex_pos);
    lua_register(L, "vertex_normal", &lua_vertex_normal);
    lua_register(L, "vertex_texture_coords", &lua_vertex_texture_coords);
    lua_register(L, "mesh_add_vertex", &lua_mesh_add_vertex);
    lua_register(L, "mesh_commit_triangle", &lua_mesh_commit_triangle);
    lua_register(L, "mesh_finalize", &lua_mesh_finalize);
    lua_register(L, "mesh_apply_material", &lua_mesh_apply_material);
    lua_register(L, "mesh_flip_x", &lua_mesh_flip_x);
    lua_register(L, "mesh_flip_y", &lua_mesh_flip_y);
    lua_register(L, "mesh_flip_z", &lua_mesh_flip_z);
    lua_register(L, "mesh_set_pos", &lua_mesh_set_pos);
    lua_register(L, "mesh_move", &lua_mesh_move);
    lua_register(L, "mesh_rotate", &lua_mesh_rotate);
    lua_register(L, "mesh_scale", &lua_mesh_scale);
    lua_register(L, "render", &lua_render);
    lua_register(L, "savebmp", &lua_savebmp);
    lua_register(L, "option", &lua_option);
    
    if (luaL_dofile(L, luaname.c_str()))
    {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }
}

static bool __lua_get_texture(lua_State* L, uint& which, uint pos=1)
{
    which = lua_tointeger(L, pos);
    
    if (which < 1 || which > lua_textures.size())
    {
        printf("lua error: texture %d does not exist\n", which);
        return false;
    }
    
    which -= 1;
    
    return true;
}

static int lua_load_texture(lua_State* L)
{
    Texture* texture = new SDLTexture(lua_tostring(L, 1));
    lua_textures.push_back(texture);
    
    lua_pushinteger(L, lua_textures.size());
    return 1;
}

static int lua_print(lua_State* L)
{
    printf("%s\n", lua_tostring(L, 1));
    return 0;
}

static int lua_scene_clear(lua_State* L)
{
    for (Object* o : scene.getObjects())
    {
        delete o;
        scene.removeObject(o);
    }
    
    for (Light* light : scene.getLights())
    {
        if (light != movableLight)
        {
            delete light;
        }
        
        scene.removeLight(light);
    }
    
    lua_lights.clear();
    
    for (Texture* tp : lua_textures)
    {
        delete tp;
    }
    
    lua_textures.clear();
    lua_materials.clear();
    lua_meshes.clear();
    lua_spheres.clear();
    
    return 0;
}

static int lua_scene_set_ambient_light(lua_State* L)
{
    scene.ambientLightPower = vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_camera_set_pos(lua_State* L)
{
    scene.camera.setPosition(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_camera_set_focal_length(lua_State* L)
{
    scene.camera.setFocalLength(lua_tonumber(L, 1));
    return 0;
}

static int lua_camera_set_pitch(lua_State* L)
{
    scene.camera.setPitch(lua_tonumber(L, 1));
    return 0;
}

static int lua_camera_set_yaw(lua_State* L)
{
    scene.camera.setYaw(lua_tonumber(L, 1));
    return 0;
}

static bool __lua_get_light(lua_State* L, uint& which)
{
    which = lua_tointeger(L, 1);
    
    if (which < 1 || which > lua_lights.size())
    {
        printf("lua error: light %d does not exist\n", which);
        return false;
    }
    
    which -= 1;
    
    return true;
}

static int lua_add_light(lua_State* L)
{
    Light* light;
    
    if (lua_lights.size() == 0)
    {
        light = movableLight;
    }
    else
    {
        light = new Light();
    }
    
    lua_lights.push_back(light);
    lua_pushinteger(L, lua_lights.size());
    
    scene.addLight(light);
    
    return 1;
}

static int lua_copy_light(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        Light* light = new Light();
        light->position = lua_lights[which]->position;
        light->diffusePower = lua_lights[which]->diffusePower;
        light->specularPower = lua_lights[which]->specularPower;
        
        lua_lights.push_back(light);
        scene.addLight(light);
        
        lua_pushinteger(L, lua_lights.size());
        
        return 1;
    }
    else
    {
        return 0;
    }
}

static int lua_light_set_pos(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        lua_lights[which]->position = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_light_set_diffuse(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        lua_lights[which]->diffusePower = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_light_set_specular(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        lua_lights[which]->specularPower = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static bool __lua_get_material(lua_State* L, uint& which, uint pos=1)
{
    which = lua_tointeger(L, pos);
    
    if (which < 1 || which > lua_materials.size())
    {
        printf("lua error: material %d does not exist\n", which);
        return false;
    }
    
    which -= 1;
    
    return true;
}

static int lua_add_material(lua_State* L)
{
    lua_materials.push_back(Material());
    lua_pushinteger(L, lua_materials.size());
    return 1;
}

static int lua_copy_material(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials.push_back(lua_materials[which]);
        lua_pushinteger(L, lua_materials.size());
        return 1;
    }
    else
    {
        return 0;
    }
}

static int lua_material_set_ambient(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].ambientFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_material_set_diffuse(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].diffuseFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_material_set_specular(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].specularFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_material_set_ambient_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].ambientTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_material_set_diffuse_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].diffuseTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_material_set_specular_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].specularTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_material_set_normal_map(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].normalMap = lua_textures[texture];
    }
    
    return 0;
}

static int lua_material_set_reflection_map(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].reflectionMap = lua_textures[texture];
    }
    
    return 0;
}

static int lua_material_set_opacity(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].opacity = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_material_set_refraction(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].refraction = lua_tonumber(L, 2);
    }
    
    return 0;
}

static int lua_material_set_reflectivity(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].reflectivity = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_material_set_metallic(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].metallic = lua_toboolean(L, 2);
    }
    
    return 0;
}

static int lua_material_set_shininess(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].shininess = lua_tonumber(L, 2);
    }
    
    return 0;
}

static bool __lua_get_sphere(lua_State* L, uint& which, uint pos=1)
{
    which = lua_tointeger(L, pos);
    
    if (which < 1 || which > lua_spheres.size())
    {
        printf("lua error: sphere %d does not exist\n", which);
        return false;
    }
    
    which -= 1;
    
    return true;
}

static int lua_add_sphere(lua_State* L)
{
    vec3 position(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    float radius = lua_tonumber(L, 4);
    uint material;
    
    if (__lua_get_material(L, material, 5))
    {
        Sphere* s = new Sphere(position, radius, lua_materials[material]);
        lua_spheres.push_back(s);
        scene.addObject(s);
        
        lua_pushinteger(L, lua_spheres.size());
        return 1;
    }
    
    return 0;
}

static int lua_sphere_set_pos(lua_State* L)
{
    uint which;
    
    if (__lua_get_sphere(L, which))
    {
        lua_spheres[which]->setPosition(vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
    }
    
    return 0;
}

static int lua_sphere_set_radius(lua_State* L)
{
    uint which;
    
    if (__lua_get_sphere(L, which))
    {
        lua_spheres[which]->setRadius(lua_tonumber(L, 2));
    }
    
    return 0;
}

static int lua_sphere_move(lua_State* L)
{
    uint which;
    
    if (__lua_get_sphere(L, which))
    {
        lua_spheres[which]->move(vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
    }
    
    return 0;
}

static int lua_sphere_rotate(lua_State* L)
{
    uint which;
    
    if (__lua_get_sphere(L, which))
    {
        vec3 axis(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
        float angle = lua_tonumber(L, 5);
        lua_spheres[which]->rotate(axis, angle);
    }
    
    return 0;
}

static int lua_sphere_scale(lua_State* L)
{
    uint which;
    
    if (__lua_get_sphere(L, which))
    {
        lua_spheres[which]->scale(float(lua_tonumber(L, 2)));
    }
    
    return 0;
}

static bool __lua_get_mesh(lua_State* L, uint& which, uint pos=1)
{
    which = lua_tointeger(L, pos);
    
    if (which < 1 || which > lua_meshes.size())
    {
        printf("lua error: mesh %d does not exist\n", which);
        return false;
    }
    
    which -= 1;
    
    return true;
}

static int lua_add_mesh(lua_State* L)
{
    Mesh* mesh = new Mesh();
    scene.addObject(mesh);
    lua_meshes.push_back(mesh);
    
    lua_pushinteger(L, lua_meshes.size());
    
    return 1;
}

static int lua_vertex_pos(lua_State* L)
{
    lua_vpos = vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_vertex_normal(lua_State* L)
{
    lua_vnormal = vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_vertex_texture_coords(lua_State* L)
{
    lua_vtexcoords = vec2(lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

static int lua_mesh_add_vertex(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->pushVertex(lua_vpos, lua_vnormal, lua_vtexcoords);
    }
    
    return 0;
}

static int lua_mesh_commit_triangle(lua_State* L)
{
    uint whichmesh;
    uint whichmat;
    
    if (__lua_get_mesh(L, whichmesh, 1) && __lua_get_material(L, whichmat, 2))
    {
        lua_meshes[whichmesh]->commitTriangle(lua_materials[whichmat]);
    }
    
    return 0;
}

static int lua_mesh_finalize(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->computeBVH();
    }
    
    return 0;
}

static int lua_mesh_apply_material(lua_State* L)
{
    uint whichmesh;
    uint whichmat;
    
    if (__lua_get_mesh(L, whichmesh, 1) && __lua_get_material(L, whichmat, 2))
    {
        lua_meshes[whichmesh]->applyMaterial(lua_materials[whichmat]);
    }
    
    return 0;
}

static int lua_mesh_flip_x(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->flipX();
    }
    
    return 0;
}

static int lua_mesh_flip_y(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->flipY();
    }
    
    return 0;
}

static int lua_mesh_flip_z(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->flipZ();
    }
    
    return 0;
}

static int lua_mesh_set_pos(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->setPosition(vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
    }
    
    return 0;
}

static int lua_mesh_move(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->move(vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
    }
    
    return 0;
}

static int lua_mesh_rotate(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        vec3 axis(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
        float angle = lua_tonumber(L, 5);
        lua_meshes[which]->rotate(axis, angle);
    }
    
    return 0;
}

static int lua_mesh_scale(lua_State* L)
{
    uint which;
    
    if (__lua_get_mesh(L, which))
    {
        lua_meshes[which]->scale(float(lua_tonumber(L, 2)));
    }
    
    return 0;
}

static int lua_load_obj(lua_State* L)
{
    OBJReader::VertexOrder order = OBJReader::VertexOrder::ABC;
    
    if (lua_gettop(L) > 1)
    {
        map<string, OBJReader::VertexOrder> vos;
        
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("ABC", OBJReader::VertexOrder::ABC));
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("ACB", OBJReader::VertexOrder::ACB));
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("BAC", OBJReader::VertexOrder::BAC));
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("BCA", OBJReader::VertexOrder::BCA));
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("CAB", OBJReader::VertexOrder::CAB));
        vos.insert(vos.end(), pair<string, OBJReader::VertexOrder>("CBA", OBJReader::VertexOrder::CBA));
        
        order = vos.at(string(lua_tostring(L, 2)));
    }
    
    Mesh* mesh = readerobj.loadFile(string(lua_tostring(L, 1)), order);
    
    mesh->computeBVH();
    lua_meshes.push_back(mesh);
    scene.addObject(mesh);
    
    lua_pushinteger(L, lua_meshes.size());
    
    return 1;
}

static int lua_load_stl(lua_State* L)
{
    Mesh* mesh = readerstl.loadStl(string(lua_tostring(L, 1)));
    
    mesh->computeBVH();
    lua_meshes.push_back(mesh);
    scene.addObject(mesh);
    
    lua_pushinteger(L, lua_meshes.size());
    
    return 1;
}

static int lua_render(lua_State* L)
{
    renderer = ray;
    single_rayrender_state = 2;
    
    gui.clear();
    ray->renderScene(scene, 640, 480, gui);
    gui.present();
    
    return 0;
}

static int lua_savebmp(lua_State* L)
{
    gui.saveBMP(lua_tostring(L, 1));
    return 0;
}

static int lua_option(lua_State* L)
{
    string which(lua_tostring(L, 1)); // "ray" or "wire"
    string optname(lua_tostring(L, 2));
    
    Renderer* r = ray;
    
    if (which == "wire")
    {
        r = wire;
    }
    
    if (lua_isboolean(L, 3))
    {
        r->setOption(Renderer::Option(optname, bool(lua_toboolean(L, 3))));
    }
    else if (lua_isinteger(L, 3))
    {
        r->setOption(Renderer::Option(optname, int(lua_tointeger(L, 3))));
    }
    else if (lua_isnumber(L, 3))
    {
        r->setOption(Renderer::Option(optname, float(lua_tonumber(L, 3))));
    }
    else
    {
        r->setOption(Renderer::Option(optname, string(lua_tostring(L, 3))));
    }
    
    return 0;
}
