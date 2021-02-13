scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(0, -2, -2.3)
camera_set_yaw(0.0)
camera_set_pitch(0.9)

light = add_light()
light_set_pos(light, 0, 0, -1)
light_set_diffuse(light, 1,1,1)
light_set_specular(light, 1,1,1)

light2 = add_light()
light_set_pos(light2, 2, 0, 2)
light_set_diffuse(light2, 100,10,10)
light_set_specular(light2, 1,1,1)

light3 = add_light()
light_set_pos(light3, -2, 0, 2)
light_set_diffuse(light3, 10,100,10)
light_set_specular(light3, 1,1,1)

kth_png = load_texture("misc/kth.png")

m1 = add_material()
material_set_ambient(m1, 0.25, 0.25, 0.25); material_set_ambient_texture(m1, kth_png)
material_set_diffuse(m1, 1,1,1); material_set_diffuse_texture(m1, kth_png)
material_set_specular(m1, 0.25, 0.25, 0.25)
material_set_shininess(m1, 50)

m2 = add_material()
material_set_ambient(m2, 0.25, 0.25, 0.25)
material_set_diffuse(m2, 0.15, 0.15, 0.15)
material_set_specular(m2, 1,1,1)
material_set_shininess(m2, 100)
material_set_reflectivity(m2, 1,1,1)

logo = add_mesh()
floor = add_mesh()

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); mesh_add_vertex(logo)
vertex_pos(-1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 0); mesh_add_vertex(logo)
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); mesh_add_vertex(logo)
mesh_commit_triangle(logo, m1)

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); mesh_add_vertex(logo)
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); mesh_add_vertex(logo)
vertex_pos(1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 1); mesh_add_vertex(logo)
mesh_commit_triangle(logo, m1)

vertex_pos(2,1,2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-2,1,2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-2,1,-2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
mesh_commit_triangle(floor, m2)

vertex_pos(2,1,2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(-2,1,-2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
vertex_pos(2,1,-2); vertex_normal(0,-1,0); mesh_add_vertex(floor)
mesh_commit_triangle(floor, m2)

mesh_finalize(logo)
mesh_finalize(floor)
