scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(0, 0, -3)
camera_set_yaw(0.0)
camera_set_pitch(0.0)

light = add_light()
light_set_pos(light, 0, -2, -12)
light_set_diffuse(light, 200,200,200)
light_set_specular(light, 1000,1000,1000)

earth2_png = load_texture("misc/earth2.png")

earthmat = add_material()
material_set_ambient(earthmat, 0.25, 0.25, 0.25); material_set_ambient_texture(earthmat, earth2_png)
material_set_diffuse(earthmat, 1,1,1); material_set_diffuse_texture(earthmat, earth2_png)
material_set_specular(earthmat, 0.25, 0.25, 0.25)
material_set_shininess(earthmat, 50)

earth = add_sphere(0,0,0, 1, earthmat)

phase = 0.0

for i=0,100 do
    sphere_rotate(earth, 1,0,0, 0.1)
    sphere_scale(earth, 1.0 + (math.sin(phase) / 100.0))
    phase = phase + 0.1
    render()
end

for i=0,100 do
    sphere_rotate(earth, 0,1,0, 0.1)
    sphere_set_radius(earth, 1.0 + (math.sin(phase) / 2.0))
    phase = phase + 0.02
    render()
end

for i=0,100 do
    sphere_rotate(earth, 0,0,1, 0.1)
    render()
end
