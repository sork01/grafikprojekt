scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

option("ray", "ShadowSamples", 16)

camera_set_pos(0, 0, -5)

ltpos_x = -1

lt = add_light()
light_set_pos(lt, ltpos_x, 0, -1.5)
light_set_diffuse(lt, 4,4,4)

m1 = add_material()
material_set_ambient(m1, 0,0,0)
material_set_diffuse(m1, 0.25, 0.25, 0.25)
material_set_specular(m1, 1,1,1)
material_set_shininess(m1, 200)
material_set_reflectivity(m1, 0.5, 0.5, 0.5)

cornellbox = load_obj("models/cornellbox.obj", "ABC")
mesh_flip_y(cornellbox)
mesh_apply_material(cornellbox, m1)

for i = 1,300 do
    render()
    savebmp("image" .. i .. ".bmp")
    print("image" .. i .. ".bmp")
    ltpos_x = ltpos_x + 0.01
    light_set_pos(lt, ltpos_x, 0, -1.5)
end
