import re

scene_location = "C:/Projects/Godot Projects/level-editor/scene.tscn"

output = "#pragma once\n"
output += "#include \"vector2.h\"\n\n"

with open(scene_location, "r") as file:
    text = file.read()
    for found in re.finditer('node name="(.*?)" type="(.*?)".*\nposition = Vector2\\((.*?), (.*?)\\)', text):
        name = found.group(1)
        type = found.group(2)
        x = float(found.group(3))
        y = -float(found.group(4))
        if type == "CollisionShape2D":
            rectangle_name = re.findall(f'node name="{name}".*\n.*\nshape = SubResource\\("(.*?)"\\)', text).pop()
            size = re.findall(f'id="{rectangle_name}".*\nsize = Vector2\\((.*?), (.*?)\\)', text).pop()
            w = float(size[0])
            h = float(size[1])
            output += "constexpr Box %s = {{%s, %s}, {%s, %s}};\n"%(name.upper(), x - w / 2, y + h / 2, x + w / 2, y - h / 2)
        else:
            output += "constexpr Vector2 %s = {%s, %s};\n"%(name.upper(), x, y)

with open("../source/position_data.h", "w") as header:
    header.write(output)
