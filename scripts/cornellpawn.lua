scene_clear()
scene_set_ambient_light(1, 1, 1)

camera_set_pos(0,0,-3.25)
camera_set_yaw(0)
camera_set_pitch(0)

lt = add_light()

light_set_pos(lt, 0,-0.6,0)
light_set_diffuse(lt, 1,1,1)
light_set_specular(lt, 1,1,1)

lt2 = add_light()

light_set_pos(lt2, 0,0.5,0)
light_set_diffuse(lt2, 1,1,1)
light_set_specular(lt2, 0,0,0)

m1 = add_material()
material_set_ambient(m1, 0.02, 0.02, 0.02)
material_set_diffuse(m1, 0.25, 0.25, 0.25)
material_set_specular(m1, 1,1,1)
material_set_shininess(m1, 2)

m2 = add_material()
material_set_ambient(m2, 0.1,0,0)
material_set_diffuse(m2, 0.25, 0.05, 0.05)
material_set_specular(m2, 3,3,3)
material_set_shininess(m2, 100)
sp1 = add_sphere(-0.5,0.75,0, 0.25, m2)

m3 = add_material()
material_set_ambient(m3, 0,0.2,0)
material_set_diffuse(m3, 0.02, 0.05, 0.02)
material_set_specular(m3, 3,3,3)
material_set_shininess(m3, 50)
material_set_reflectivity(m3, 0.6, 0.6, 0.6)
sp2 = add_sphere(-0.1,0.7,0.5, 0.3, m3)

m4 = add_material()
material_set_ambient(m4, 0,0,0.2)
material_set_diffuse(m4, 0.02, 0.02, 0.5)
material_set_specular(m4, 12,12,12)
material_set_shininess(m4, 50)
material_set_opacity(m4, 0.32, 0.32, 0.32)
-- material_set_reflectivity(m4, 0.5, 0.5, 0.5)
material_set_refraction(m4, 1.16)
sp3 = add_sphere(0.4,0.65,-0.5, 0.35, m4)

cornellbox = load_obj("models/cornellempty.obj", "ABC")
mesh_apply_material(cornellbox, m1)

pawn = load_obj("models/pawn.obj", "ABC")
mesh_flip_y(pawn)
mesh_scale(pawn, 0.25)
mesh_move(pawn, 0, 0, -1.5)
mesh_apply_material(pawn, m4)
