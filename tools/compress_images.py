import subprocess
import os

oxipng_path = "C:/oxipng/oxipng.exe"

for (root, dirs, files) in os.walk("..\\assets"):
    for file in files:
        split = file.split('.', 1)
        extension = split[1]
        full_path = os.path.join(root, file);

        if extension == "png":
            subprocess.run([oxipng_path, full_path, "--alpha", "-o", "max", "-z"], shell=True)
            print(full_path)
