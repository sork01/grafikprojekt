scene_clear()
scene_set_ambient_light(0.7, 0.7, 0.7, 0.7)

lt = add_light()
light_set_pos(lt, -6,-4.2,11)
light_set_diffuse(lt, 120, 120, 120)
light_set_specular(lt, 150, 150, 150)

lt2 = add_light()
light_set_pos(lt2, -1,-4.2,11)
light_set_diffuse(lt2, 120, 120, 120)
light_set_specular(lt2, 150, 150, 150)

camera_set_focal_length(600)
camera_set_pos(4,-7.2,17)
camera_set_pitch(0.3)
camera_set_yaw(2.9)

bunnymat = add_material()
material_set_ambient(bunnymat, 0.25, 0.25, 0.25)
material_set_diffuse(bunnymat, 0.25, 0.25, 0.25)
material_set_specular(bunnymat, 0.25, 0.25, 0.25)
material_set_shininess(bunnymat, 100)
-- material_set_opacity(bunnymat, 0.4, 0.4, 0.4)
-- material_set_reflectivity(bunnymat, 0.8, 0.8, 0.8)

bunnymesh = load_obj("models/superbunny.obj", "ABC")
mesh_apply_material(bunnymesh, bunnymat)

floormat = add_material()
material_set_ambient(floormat, 0.15, 0.15, 0.15)
material_set_diffuse(floormat, 0.25, 0.25, 0.25)
material_set_specular(floormat, 0.25, 0.25, 0.25)
material_set_shininess(floormat, 100)

floormesh = add_mesh()

scale = 50
y = 3.3

a = {scale, y, scale}
b = {-scale, y, scale}
c = {-scale, y, -scale}
d = {scale, y, -scale}

vertex_pos(a[1],a[2],a[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
vertex_pos(b[1],b[2],b[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
vertex_pos(c[1],c[2],c[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
mesh_commit_triangle(floormesh, floormat)

vertex_pos(d[1],d[2],d[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
vertex_pos(a[1],a[2],a[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
vertex_pos(c[1],c[2],c[3]); vertex_normal(0,-1,0); mesh_add_vertex(floormesh)
mesh_commit_triangle(floormesh, floormat)

mesh_finalize(floormesh);

wallmat = add_material()
material_set_ambient(wallmat, 0.1, 0.1, 0.1)
material_set_diffuse(wallmat, 0.25, 0.25, 0.25)
material_set_specular(wallmat, 0.25, 0.25, 0.25)
material_set_shininess(wallmat, 100)

wallsphere = add_sphere(0,0,0, scale, wallmat)
-- render()
-- savebmp("bunny.bmp")
