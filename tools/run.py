import subprocess
import os

def run_file(filename : str):
    exec(open(filename).read())

print("started")

while True:
    print(" ")
    command = input()
    command = command.strip()
    command = ''.join(filter(str.isalnum, command)) # Remove non-alphanumeric

    if command == "compress":
        run_file("compress_images.py")
    elif command == "export":
        run_file("export_aseprite.py")
        run_file("import_assets.py")
    elif command == "image":
        run_file("import_assets.py")
    elif command == "dialog":
        run_file("import_dialog.py")
    elif command == "build":
        os.chdir("../build")
        subprocess.call(os.path.abspath("../build/build.bat"), shell=True)
        os.chdir("../tools")
    elif command.startswith("new "):
        name = command.removeprefix("new ")
        if os.path.isfile(f"../source/{name}.cpp") or os.path.isfile(f"../source/{name}.h"):
            print("file already exists")
            continue
        else:
            with open(f"../source/{name}.h", "w") as h:
                h.write("#pragma once\n")
            with open(f"../source/{name}.cpp", "w") as cpp:
                cpp.write(f"#include \"{name}.h\"\n")
    elif command.startswith("delete "):
        name = command.removeprefix("delete ")
        if not os.path.isfile(f"../source/{name}.cpp") or not os.path.isfile(f"../source/{name}.h"):
            print("file doesn't exist")
            continue
        else:
            os.remove(f"../source/{name}.cpp")
            os.remove(f"../source/{name}.h")
    elif command == "help":
        print("compress")
        print("export")
        print("image")
        print("dialog")
        print("build")
        print("new <filename>")
        print("delete <filename>")
        print("stop")
        continue
    elif command == "stop":
        break
    else:
        print("unknown command")
        continue

    print("done")
