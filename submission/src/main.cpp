
// *neode.onsave* setgo gcc -pthread -I/usr/include/lua5.3 -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glm -I/usr/include/SDL2 -fsyntax-only main.cpp 

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
Mesh* lua_current_mesh;
vec3 lua_vpos;
vec3 lua_vnormal;
vec2 lua_vtexcoords;
string luaname = "script.lua";

static int lua_load_texture(lua_State* L);
static int lua_print(lua_State* L);
static int lua_clear_scene(lua_State* L);
static int lua_set_ambient_light(lua_State* L);
static int lua_set_camera_pos(lua_State* L);
static int lua_set_camera_focal_length(lua_State* L);
static int lua_set_camera_pitch(lua_State* L);
static int lua_set_camera_yaw(lua_State* L);
static int lua_add_light(lua_State* L);
static int lua_copy_light(lua_State* L);
static int lua_set_light_pos(lua_State* L);
static int lua_set_light_diffuse(lua_State* L);
static int lua_set_light_specular(lua_State* L);
static int lua_add_material(lua_State* L);
static int lua_copy_material(lua_State* L);
static int lua_set_material_ambient(lua_State* L);
static int lua_set_material_diffuse(lua_State* L);
static int lua_set_material_specular(lua_State* L);
static int lua_set_material_ambient_texture(lua_State* L);
static int lua_set_material_diffuse_texture(lua_State* L);
static int lua_set_material_specular_texture(lua_State* L);
static int lua_set_material_opacity(lua_State* L);
static int lua_set_material_refraction(lua_State* L);
static int lua_set_material_reflectivity(lua_State* L);
static int lua_set_material_metallic(lua_State* L);
static int lua_set_material_shininess(lua_State* L);
static int lua_add_sphere(lua_State* L);
static int lua_begin_mesh(lua_State* L);
static int lua_vertex_pos(lua_State* L);
static int lua_vertex_normal(lua_State* L);
static int lua_vertex_texture_coords(lua_State* L);
static int lua_add_vertex(lua_State* L);
static int lua_commit_triangle(lua_State* L);
static int lua_commit_mesh(lua_State* L);
static int lua_load_obj(lua_State* L);
static int lua_load_obj_using_material(lua_State* L);
static int lua_load_stl(lua_State* L);
static int lua_load_stl_using_material(lua_State* L);
static int lua_render(lua_State* L);
static int lua_savebmp(lua_State* L);

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
    
    renderer = wire;
    
    bool quit = false;
    
    while (!quit)
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
            
            printf("render time: %.1f ms  (%.1f potential fps)\n", (time2 - time1)*1000.0, 1000.0/((time2 - time1)*1000.0));
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
    lua_register(L, "clear_scene", &lua_clear_scene);
    lua_register(L, "set_ambient_light", &lua_set_ambient_light);
    lua_register(L, "set_camera_pos", &lua_set_camera_pos);
    lua_register(L, "set_camera_focal_length", &lua_set_camera_focal_length);
    lua_register(L, "set_camera_pitch", &lua_set_camera_pitch);
    lua_register(L, "set_camera_yaw", &lua_set_camera_yaw);
    lua_register(L, "load_stl", &lua_load_stl);
    lua_register(L, "load_stl_using_material", &lua_load_stl_using_material);
    lua_register(L, "load_obj", &lua_load_obj);
    lua_register(L, "load_obj_using_material", &lua_load_obj_using_material);
    lua_register(L, "add_light", &lua_add_light);
    lua_register(L, "copy_light", &lua_copy_light);
    lua_register(L, "set_light_pos", &lua_set_light_pos);
    lua_register(L, "set_light_diffuse", &lua_set_light_diffuse);
    lua_register(L, "set_light_specular", &lua_set_light_specular);
    lua_register(L, "add_material", &lua_add_material);
    lua_register(L, "copy_material", &lua_copy_material);
    lua_register(L, "load_texture", &lua_load_texture);
    lua_register(L, "set_material_ambient", &lua_set_material_ambient);
    lua_register(L, "set_material_diffuse", &lua_set_material_diffuse);
    lua_register(L, "set_material_specular", &lua_set_material_specular);
    lua_register(L, "set_material_ambient_texture", &lua_set_material_ambient_texture);
    lua_register(L, "set_material_diffuse_texture", &lua_set_material_diffuse_texture);
    lua_register(L, "set_material_specular_texture", &lua_set_material_specular_texture);
    lua_register(L, "set_material_opacity", &lua_set_material_opacity);
    lua_register(L, "set_material_refraction", &lua_set_material_refraction);
    lua_register(L, "set_material_reflectivity", &lua_set_material_reflectivity);
    lua_register(L, "set_material_metallic", &lua_set_material_metallic);
    lua_register(L, "set_material_shininess", &lua_set_material_shininess);
    lua_register(L, "add_sphere", &lua_add_sphere);
    lua_register(L, "begin_mesh", &lua_begin_mesh);
    lua_register(L, "vertex_pos", &lua_vertex_pos);
    lua_register(L, "vertex_normal", &lua_vertex_normal);
    lua_register(L, "vertex_texture_coords", &lua_vertex_texture_coords);
    lua_register(L, "add_vertex", &lua_add_vertex);
    lua_register(L, "commit_triangle", &lua_commit_triangle);
    lua_register(L, "commit_mesh", &lua_commit_mesh);
    lua_register(L, "render", &lua_render);
    lua_register(L, "savebmp", &lua_savebmp);
    luaL_dofile(L, luaname.c_str());
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

static int lua_clear_scene(lua_State* L)
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
    
    return 0;
}

static int lua_set_ambient_light(lua_State* L)
{
    scene.ambientLightPower = vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_set_camera_pos(lua_State* L)
{
    scene.camera.setPosition(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    return 0;
}

static int lua_set_camera_focal_length(lua_State* L)
{
    scene.camera.setFocalLength(lua_tonumber(L, 1));
    return 0;
}

static int lua_set_camera_pitch(lua_State* L)
{
    scene.camera.setPitch(lua_tonumber(L, 1));
    return 0;
}

static int lua_set_camera_yaw(lua_State* L)
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

static int lua_set_light_pos(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        lua_lights[which]->position = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_light_diffuse(lua_State* L)
{
    uint which;
    
    if (__lua_get_light(L, which))
    {
        lua_lights[which]->diffusePower = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_light_specular(lua_State* L)
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

static int lua_set_material_ambient(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].ambientFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_material_diffuse(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].diffuseFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_material_specular(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].specularFactors = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_material_ambient_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].ambientTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_set_material_diffuse_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].diffuseTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_set_material_specular_texture(lua_State* L)
{
    uint material;
    uint texture;
    
    if (__lua_get_material(L, material, 1) && __lua_get_texture(L, texture, 2))
    {
        lua_materials[material].specularTexture = lua_textures[texture];
    }
    
    return 0;
}

static int lua_set_material_opacity(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].opacity = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_material_refraction(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].refraction = lua_tonumber(L, 2);
    }
    
    return 0;
}

static int lua_set_material_reflectivity(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].reflectivity = vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
    return 0;
}

static int lua_set_material_metallic(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].metallic = lua_toboolean(L, 2);
    }
    
    return 0;
}

static int lua_set_material_shininess(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_materials[which].shininess = lua_tonumber(L, 2);
    }
    
    return 0;
}

static int lua_add_sphere(lua_State* L)
{
    vec3 position(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    float radius = lua_tonumber(L, 4);
    uint material;
    
    if (__lua_get_material(L, material, 5))
    {
        scene.addObject(new Sphere(position, radius, lua_materials[material]));
    }
    
    return 0;
}

static int lua_begin_mesh(lua_State* L)
{
    lua_current_mesh = new Mesh();
    return 0;
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

static int lua_add_vertex(lua_State* L)
{
    lua_current_mesh->pushVertex(lua_vpos, lua_vnormal, lua_vtexcoords);
    return 0;
}

static int lua_commit_triangle(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which))
    {
        lua_current_mesh->commitTriangle(lua_materials[which]);
    }
    
    return 0;
}

static int lua_commit_mesh(lua_State* L)
{
    lua_current_mesh->computeBVH();
    scene.addObject(lua_current_mesh);
    return 0;
}

static int lua_load_obj(lua_State* L)
{
    Mesh* mesh = readerobj.loadFile(string(lua_tostring(L, 1)));
    mesh->flipY();
    mesh->computeBVH();
    scene.addObject(mesh);
    
    return 0;
}

static int lua_load_obj_using_material(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which, 2))
    {
        Mesh* mesh = readerobj.loadFile(string(lua_tostring(L, 1)));
        mesh->flipY();
        mesh->computeBVH();
        mesh->applyMaterial(lua_materials[which]);
        scene.addObject(mesh);
    }
    
    return 0;
}

static int lua_load_stl(lua_State* L)
{
    Mesh* mesh = readerstl.loadStl(string(lua_tostring(L, 1)));
    mesh->flipY();
    mesh->computeBVH();
    scene.addObject(mesh);
    
    return 0;
}

static int lua_load_stl_using_material(lua_State* L)
{
    uint which;
    
    if (__lua_get_material(L, which, 2))
    {
        Mesh* mesh = readerstl.loadStl(string(lua_tostring(L, 1)));
        mesh->flipY();
        mesh->computeBVH();
        mesh->applyMaterial(lua_materials[which]);
        scene.addObject(mesh);
    }
    
    return 0;
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
