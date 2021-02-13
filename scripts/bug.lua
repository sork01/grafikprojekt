-- the bug is visible in commit 24b210a32b4d29739f850bfcb4d29ca24f40acff
scene_clear()
scene_set_ambient_light(0.25, 0.25, 0.25)

camera_set_pos(-3, 0, -3.001)
camera_set_yaw(0.5)
camera_set_pitch(0.0)

light = add_light()
light_set_pos(light, 0, 0, 2)
light_set_diffuse(light, 25,25,25)
light_set_specular(light, 25,25,25)


-- pawnmat = add_material()
-- material_set_ambient(pawnmat, 0.25, 0.25, 0.25)
-- material_set_diffuse(pawnmat, 0.45, 0.45, 1.45)
-- material_set_specular(pawnmat, 1,1,1)
-- material_set_shininess(pawnmat, 100)
-- material_set_reflectivity(pawnmat, 1,1,1)

-- pawn = load_obj("models/pawn.obj", "ABC")
-- mesh_flip_y(pawn)
-- mesh_apply_material(pawn, pawnmat)
-- mesh_move(pawn, 0, 0, 8)

spheremat = add_material()
material_set_ambient(spheremat, 0.25, 0.15, 0.15)
material_set_diffuse(spheremat, 1.85, 0.45, 0.45)
material_set_specular(spheremat, 4,4,4)
material_set_shininess(spheremat, 100)
material_set_reflectivity(spheremat, 1,1,1)
-- material_set_metallic(spheremat, true)

sp1 = add_sphere(-16,0,8, 9, spheremat)
sp2 = add_sphere(-12,0,35, 9, spheremat)

render()

-- for i=0,100 do
    -- mesh_rotate(pawn, 1, 1, 0, 0.2)
    -- render()
-- end
