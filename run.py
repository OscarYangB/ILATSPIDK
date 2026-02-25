import subprocess
import os

def run_tool(filename : str):
    os.chdir("./tools")
    subprocess.call(["python", filename])
    os.chdir("../")

def text_filter(str):
    return str.isalnum or str == ' '

print("started")

while True:
    print(" ")
    command = input()
    command = command.strip()
    command = ''.join(filter(text_filter, command)) # Remove non-alphanumeric

    if command == "compress" or command == "c":
        run_tool("compress_images.py")
    elif command == "export" or command == "e":
        run_tool("export_aseprite.py")
        run_tool("import_assets.py")
    elif command == "image" or command == "i":
        run_tool("import_assets.py")
    elif command == "dialog" or command == "d":
        run_tool("import_dialog.py")
    elif command == "build" or command == "b":
        os.chdir("./build")
        subprocess.call(os.path.abspath("build.bat"), shell=True)
        os.chdir("../")
    elif command.startswith("new ") or command.startswith("n "):
        name = command.removeprefix("new ")
        name = name.removeprefix("n ")
        if os.path.isfile(f"./source/{name}.cpp") or os.path.isfile(f"./source/{name}.h"):
            print("file already exists")
            continue
        else:
            with open(f"./source/{name}.h", "w") as h:
                h.write("#pragma once\n")
            with open(f"./source/{name}.cpp", "w") as cpp:
                cpp.write(f"#include \"{name}.h\"\n")
    elif command.startswith("remove ") or command.startswith("r "):
        name = command.removeprefix("remove ")
        name = name.removeprefix("r ")
        if not os.path.isfile(f"./source/{name}.cpp") or not os.path.isfile(f"./source/{name}.h"):
            print("file doesn't exist")
            continue
        else:
            os.remove(f"./source/{name}.cpp")
            os.remove(f"./source/{name}.h")
    elif command == "help" or command == "h":
        print("(c)ompress")
        print("(e)xport")
        print("(i)mage")
        print("(d)ialog")
        print("(b)uild")
        print("(n)ew <filename>")
        print("(r)emove <filename>")
        print("(s)top")
        continue
    elif command == "stop" or command == "s":
        break
    else:
        print("unknown command")
        continue

    print("done")
