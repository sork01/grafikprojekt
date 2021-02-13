clear_scene()
set_ambient_light(1, 1, 1)

-- load_obj("../models/cornellbox.obj")

L1 = add_light()

set_light_pos(L1, -1.0, -1.0, -2.0)
set_light_diffuse(L1, 4.0, 1.0, 1.0)
set_light_specular(L1, 12.0, 12.0, 12.0)

L2 = copy_light(L1)
set_light_pos(L2, 1.0, 1.0, -2.0)

M1 = add_material()
set_material_ambient(M1, 0.1, 0.1, 0.1)
set_material_diffuse(M1, 1.5, 1.5, 1.5)
set_material_specular(M1, 1, 1, 1)
set_material_shininess(M1, 100)

M2 = copy_material(M1)
set_material_diffuse(M2, 0.5, 0.5, 0.5)

add_sphere(0, 0, 0, 1.0, M1)
add_sphere(-1, 0, -1, 0.4, M2)
add_sphere(0, -1, -1, 0.4, M2)
