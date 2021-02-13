scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(1.17, -0.42, -2.01)
camera_set_yaw(0.54)
camera_set_pitch(0.26)

light = add_light()
light_set_pos(light, 0, -2, -12)
light_set_diffuse(light, 200,200,200)
light_set_specular(light, 200,200,200)

dkmat = add_material()
material_set_ambient(dkmat, 0.5,0.5,0.5)
material_set_diffuse(dkmat, 0.5,0.5,0.5)
material_set_specular(dkmat, 0.01,0.01,0.01)

dk = load_stl("models/dk.stl")
mesh_apply_material(dk, dkmat)
mesh_scale(dk, 0.025)
mesh_rotate(dk, 1,0,0, 3.1415/2.0)
mesh_move(dk, 0,1,0)
