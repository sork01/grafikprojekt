scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(0, 0, -2.3)
camera_set_yaw(0.0)
camera_set_pitch(0.0)

light = add_light()
light_set_pos(light, 0, 0, -1)
light_set_diffuse(light, 1,1,1)
light_set_specular(light, 1,1,1)

kth_png = load_texture("misc/kth.png")
kth_normal = load_texture("misc/kth-normal.png")

m1 = add_material()
material_set_ambient(m1, 0.25, 0.25, 0.25); material_set_ambient_texture(m1, kth_png)
material_set_diffuse(m1, 1,1,1); material_set_diffuse_texture(m1, kth_png)
material_set_specular(m1, 0.25, 0.25, 0.25)
material_set_normal_map(m1, kth_normal)
material_set_shininess(m1, 50)

logo = add_mesh()

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); mesh_add_vertex(logo)
vertex_pos(-1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 0); mesh_add_vertex(logo)
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); mesh_add_vertex(logo)
mesh_commit_triangle(logo, m1)

vertex_pos(1, -1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 0); mesh_add_vertex(logo)
vertex_pos(-1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(0, 1); mesh_add_vertex(logo)
vertex_pos(1, 1, 0); vertex_normal(0, 0, -1); vertex_texture_coords(1, 1); mesh_add_vertex(logo)
mesh_commit_triangle(logo, m1)

mesh_finalize(logo)
mesh_rotate(logo, 1,1,1, 0.6)

sph = add_sphere(-0.6,0.4,-0.15, 0.25, m1)
sphere_rotate(sph, 0,1,0, -1.2)
