import re

scene_location = "C:/Projects/Godot Projects/level-editor/scene.tscn"

output = "#pragma once\n"
output += "#include \"vector2.h\"\n\n"

with open(scene_location, "r") as file:
    text = file.read()
    for found in re.finditer('node name="(.*?)".*\nposition = Vector2\\((.*?), (.*?)\\)', text):
        name = found.group(1)
        x : int = int(found.group(2))
        y : int= -int(found.group(3))
        output += "constexpr Vector2 %s = {%s, %s};\n"%(name.upper(), x, y)

with open("../source/position_data.h", "w") as header:
    header.write(output)
