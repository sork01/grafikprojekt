clear_scene()
set_ambient_light(1, 1, 1)

set_camera_pos(0,0,-3)
set_camera_yaw(0)
set_camera_pitch(0)

lt = add_light()

set_light_pos(lt, 0,-0.6,0)
set_light_diffuse(lt, 1,1,1)
set_light_specular(lt, 1,1,1)

lt2 = add_light()

set_light_pos(lt2, 0,0.5,0)
set_light_diffuse(lt2, 1,1,1)
set_light_specular(lt2, 0,0,0)

m1 = add_material()
set_material_ambient(m1, 0.02, 0.02, 0.02)
set_material_diffuse(m1, 0.25, 0.25, 0.25)
set_material_specular(m1, 1,1,1)
set_material_shininess(m1, 2)

m2 = add_material()
set_material_ambient(m2, 0.1,0,0)
set_material_diffuse(m2, 0.25, 0.05, 0.05)
set_material_specular(m2, 3,3,3)
set_material_shininess(m2, 100)
add_sphere(-0.5,0.75,0, 0.25, m2)

m3 = add_material()
set_material_ambient(m3, 0,0.2,0)
set_material_diffuse(m3, 0.02, 0.05, 0.02)
set_material_specular(m3, 3,3,3)
set_material_shininess(m3, 50)
set_material_reflectivity(m3, 0.6, 0.6, 0.6)
add_sphere(-0.1,0.7,0.5, 0.3, m3)

m4 = add_material()
set_material_ambient(m4, 0,0,0.2)
set_material_diffuse(m4, 0.02, 0.02, 0.5)
set_material_specular(m4, 12,12,12)
set_material_shininess(m4, 50)
set_material_opacity(m4, 0.2, 0.2, 0.2)
set_material_reflectivity(m4, 0.5, 0.5, 0.5)
add_sphere(0.4,0.65,-0.5, 0.35, m4)

load_obj_using_material("../models/cornellempty.obj", m1)