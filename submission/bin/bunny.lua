clear_scene()
set_ambient_light(1,1,1)

lt = add_light()
set_light_pos(lt, 0,0,0)

set_camera_focal_length(600)
set_camera_pos(4,-1.3,6)
set_camera_pitch(0)
set_camera_yaw(2.8)

mat = add_material()
set_material_ambient(mat, 0.25, 0.25, 0.25)
set_material_diffuse(mat, 0.25, 0.25, 0.25)
set_material_specular(mat, 0.25, 0.25, 0.25)
set_material_shininess(mat, 100)

load_obj_using_material("/home/lur/obj/bunnynormals.obj", mat)
