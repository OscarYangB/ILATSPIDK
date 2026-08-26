from cpp_utils import *
import json
from PIL import Image
import numpy as np

embed_builder = EmbedBuilder("image_file")
file_enum_builder = EnumBuilder("ImageFile")
sprite_enum_builder = EnumBuilder("Sprite")
transform_array_builder = ArrayBuilder("SpriteAtlasTransform", "sprite_atlas_transform")
sprite_to_image_array_builder = ArrayBuilder("ImageFile", "sprite_to_image_file")
image_size_array_builder = ArrayBuilder("ImageDimensions", "image_dimensions");

files = get_files_of_type("png")

def calculate_image_centroid(image : Image.Image):
    image_alpha_data = np.asarray(image.convert('RGBA').getchannel('A'))
    (X, Y) = image.size
    image_alpha_data = image_alpha_data / np.sum(np.sum(image_alpha_data))
    x_sum = np.sum(image_alpha_data, 0)
    y_sum = np.sum(image_alpha_data, 1)
    center_x = np.sum(x_sum * np.arange(X))
    center_y = np.sum(y_sum * np.arange(Y))
    return (int(np.round(center_x)), int(np.round(center_y)))

for (name, path) in files:
    embed_builder.add_embed(path)
    asset_name = name.upper() + "_IMAGE"
    file_enum_builder.add_entry(asset_name)

    json_path = path.replace("png", "json")
    if os.path.isfile(json_path):
        with open(json_path) as data:
            data = json.load(data)
            list = data["frames"]
            image_size_array_builder.add_entry("{%s, %s}"%(list[0]["sourceSize"]["w"], list[0]["sourceSize"]["h"]))
            for entry in list:
                image_name = entry["filename"].upper().replace(" ", "_")
                image_name = image_name.replace("LAYER_1_", "")
                image_name = image_name.replace("__", "_")
                image_x = entry["frame"]["x"]
                image_y = entry["frame"]["y"]
                image_w = entry["frame"]["w"]
                image_h = entry["frame"]["h"]
                visible_x = entry["spriteSourceSize"]["x"]
                visible_y = entry["spriteSourceSize"]["y"]
                visible_w = entry["spriteSourceSize"]["w"]
                visible_h = entry["spriteSourceSize"]["h"]
                (centroid_x, centroid_y) = calculate_image_centroid(Image.open(path).crop((image_x, image_y, image_x + image_w, image_y + image_h)))

                sprite_enum_builder.add_entry(image_name)
                transform_array_builder.add_entry("{%s, %s, %s, %s, %s, %s, %s, %s, %s, %s}"%(image_x,image_y,image_w,image_h,visible_x,visible_y,visible_x+visible_w,visible_y+visible_h,centroid_x,centroid_y))
                sprite_to_image_array_builder.add_entry("ImageFile::%s"%(asset_name))
    else:
        sprite_enum_builder.add_entry(name.upper())
        image = Image.open(path)
        aabb = image.getbbox()
        dimensions = image.size
        (centroid_x, centroid_y) = calculate_image_centroid(image)
        assert aabb is not None
        transform_array_builder.add_entry("{%s, %s, %s, %s, %s, %s, %s, %s, %s, %s}"%(0, 0, image.size[0], image.size[1],aabb[0],aabb[1],aabb[2],aabb[3], centroid_x, centroid_y))
        sprite_to_image_array_builder.add_entry("ImageFile::%s"%(asset_name))
        image_size_array_builder.add_entry("{%s, %s}"%(dimensions[0], dimensions[1]))

sprite_enum_builder.add_entry("NONE")

text : str = build_header(
    [embed_builder, file_enum_builder, image_size_array_builder, sprite_enum_builder, transform_array_builder, sprite_to_image_array_builder, f"constexpr int NUMBER_OF_IMAGES = {len(files)};"],
    ["image_structures.h"])

with open("../source/data/image_data.h", "w") as header:
    header.write(text)
