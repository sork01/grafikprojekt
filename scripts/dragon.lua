scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(0.2, -1.3, -3)
camera_set_yaw(0.0)
camera_set_pitch(0.5)

light = add_light()
light_set_pos(light, 0, -2, -12)
light_set_diffuse(light, 200,200,200)
light_set_specular(light, 200,200,200)

dragonmat = add_material()
material_set_ambient(dragonmat, 1, 0.3, 0.3)
material_set_diffuse(dragonmat, 0.47, 0.1, 0.1)
material_set_specular(dragonmat, 1, 1, 1)
material_set_shininess(dragonmat, 30)

dragon = load_stl("models/dragon.stl")
mesh_apply_material(dragon, dragonmat)
mesh_scale(dragon, 0.04)
mesh_rotate(dragon, 1,0,0, 3.1415/2.0)
mesh_move(dragon, 0,1,0)
