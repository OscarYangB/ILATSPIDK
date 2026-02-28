import subprocess

aseprite_path = "C:/Program Files/Aseprite/Aseprite.exe"
print("name:", end=" ")
name = input()
subprocess.run([aseprite_path,
                "--sheet", "C:/Projects/ILATSPIDK/assets/atlas/%s.png"%name,
                "--data", "C:/Projects/ILATSPIDK/assets/atlas/%s.json"%name,
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
