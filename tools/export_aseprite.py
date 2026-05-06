import subprocess

aseprite_path = "C:/Program Files/Aseprite/Aseprite.exe"
godot_project_path = "C:/Projects/Godot Projects/level-editor/"

print("name:", end=" ")
name = input()
subprocess.run([aseprite_path,
                "--sheet", "C:/Projects/ILATSPIDK/assets/art/%s.png"%name,
                "--data", "C:/Projects/ILATSPIDK/assets/art/%s.json"%name,
                "--sheet-pack",
                "--split-layers",
                "--all-layers",
                "--ignore-empty",
                "--format", "json-array",
                "--list-layer-hierarchy",
                "--batch",
                "--filename-format",
                "{title}_{group}_{layer}_{frame1}",
                "--ignore-layer", "Reference",
                "C:/Projects/Aseprite Projects/%s.aseprite"%name], shell=True)
subprocess.run([aseprite_path,
                "--sheet", godot_project_path + "%s.png"%name,
                "--data", godot_project_path + "image_json_data/%s.json"%name,
                "--all-layers",
                "--ignore-empty",
                "--batch",
                "--oneframe",
                "C:/Projects/Aseprite Projects/%s.aseprite"%name], shell=True)
