# Requires SDL shadercross

import os
import subprocess

shadercross_path = "C:/shadercross/bin/shadercross.exe";
formats = ["spv", "msl", "dxil"]

for (root, dirs, files) in os.walk("..\\shaders"):
    for file in files:
        full_path = os.path.abspath(os.path.join(root, file))

        if file.endswith("hlsl"):
            for format in formats:
                path = f"../assets/shaders/{format}"
                if not os.path.exists(path): os.makedirs(path)
                subprocess.run([shadercross_path, full_path, "-o", f"{path}/{file}.{format}"], shell=True)
