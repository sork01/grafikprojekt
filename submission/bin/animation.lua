clear_scene()
set_ambient_light(0.25, 0.25, 0.25)

set_camera_pos(0, 0, -5)

ltpos_x = -1

lt = add_light()
set_light_pos(lt, ltpos_x, 0, -1.5)
set_light_diffuse(lt, 4,4,4)

m1 = add_material()
set_material_ambient(m1, 0,0,0)
set_material_diffuse(m1, 0.25, 0.25, 0.25)
set_material_specular(m1, 1,1,1)
set_material_shininess(m1, 200)
-- set_material_reflectivity(m1, 0.5, 0.5, 0.5)

load_obj_using_material("../models/cornellbox.obj", m1)

for i = 1,300 do
    render()
    savebmp("image" .. i .. ".bmp")
    print("image" .. i .. ".bmp")
    ltpos_x = ltpos_x + 0.01
    set_light_pos(lt, ltpos_x, 0, -1.5)
end
