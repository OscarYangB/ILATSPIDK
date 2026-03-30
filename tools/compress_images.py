import subprocess
from cpp_utils import get_files_of_type

oxipng_path = "C:/oxipng/oxipng.exe"

# Should do for jpeg too with jpegoptim

# Needs to be in a different directory since this would be lossy compression

# Quality benchmark that I did with a test jpeg
# 95% is almost indistiguishable -> 70% reduction
# 90%/75% you can sort of tell -> 80%/90% reduction
# 50% you can tell -> 95% reduction
# 25% very blurry -> 97% reduction
# 10% looks deep fried very cool! -> 98% reduction
# 5% new and exciting patterns emergse -> 99% reduction
# 1% blocky pixel art -> 99% reduction

for (name, path) in get_files_of_type("png"):
    subprocess.run([oxipng_path, path, "--alpha", "-o", "max", "-z"], shell=True)
    print(path)
