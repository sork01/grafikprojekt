clear_scene()
set_ambient_light(0.25, 0.25, 0.25)

set_camera_pos(0, -2, -2.3)
set_camera_yaw(0.0)
set_camera_pitch(0.9)

light = add_light()
set_light_pos(light, 0, 0, -1)
set_light_diffuse(light, 1,1,1)
set_light_specular(light, 1,1,1)

light2 = add_light()
set_light_pos(light2, 2, 0, 2)
set_light_diffuse(light2, 100,10,10)
set_light_specular(light2, 1,1,1)

light3 = add_light()
set_light_pos(light3, -2, 0, 2)
set_light_diffuse(light3, 10,100,10)
set_light_specular(light3, 1,1,1)

kth_png = load_texture("../misc/kth.png")

m1 = add_material()
set_material_ambient(m1, 0.25, 0.25, 0.25); set_material_ambient_texture(m1, kth_png)
set_material_diffuse(m1, 1,1,1); set_material_diffuse_texture(m1, kth_png)
set_material_specular(m1, 0.25, 0.25, 0.25)
set_material_shininess(m1, 50)

m2 = add_material()
set_material_ambient(m2, 0.25, 0.25, 0.25)
set_material_diffuse(m2, 0.15, 0.15, 0.15)
set_material_specular(m2, 1,1,1)
set_material_shininess(m2, 100)
set_material_reflectivity(m2, 1,1,1)

begin_mesh()

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); add_vertex()
vertex_pos(-1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 0); add_vertex()
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); add_vertex()
commit_triangle(m1)

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); add_vertex()
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); add_vertex()
vertex_pos(1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 1); add_vertex()
commit_triangle(m1)

vertex_pos(2,1,2); vertex_normal(0,-1,0); add_vertex()
vertex_pos(-2,1,2); vertex_normal(0,-1,0); add_vertex()
vertex_pos(-2,1,-2); vertex_normal(0,-1,0); add_vertex()
commit_triangle(m2)

vertex_pos(2,1,2); vertex_normal(0,-1,0); add_vertex()
vertex_pos(-2,1,-2); vertex_normal(0,-1,0); add_vertex()
vertex_pos(2,1,-2); vertex_normal(0,-1,0); add_vertex()
commit_triangle(m2)

commit_mesh()
