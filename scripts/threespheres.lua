scene_clear()
scene_set_ambient_light(1, 1, 1)

L1 = add_light()

light_set_pos(L1, -1.0, -1.0, -2.0)
light_set_diffuse(L1, 4.0, 1.0, 1.0)
light_set_specular(L1, 12.0, 12.0, 12.0)

L2 = copy_light(L1)
light_set_pos(L2, 1.0, 1.0, -2.0)

M1 = add_material()
material_set_ambient(M1, 0.1, 0.1, 0.1)
material_set_diffuse(M1, 1.5, 1.5, 1.5)
material_set_specular(M1, 1, 1, 1)
material_set_shininess(M1, 100)

M2 = copy_material(M1)
material_set_diffuse(M2, 0.5, 0.5, 0.5)

s1 = add_sphere(0, 0, 0, 1.0, M1)
s2 = add_sphere(-1, 0, -1, 0.4, M2)
s3 = add_sphere(0, -1, -1, 0.4, M2)
