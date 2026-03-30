from cpp_utils import *
import json
from PIL import Image

embed_builder = EmbedBuilder("image_file")
file_enum_builder = EnumBuilder("ImageFile")
sprite_enum_builder = EnumBuilder("Sprite")
transform_array_builder = ArrayBuilder("SpriteAtlasTransform", "sprite_atlas_transform")
sprite_to_image_array_builder = ArrayBuilder("ImageFile", "sprite_to_image_file")

files = get_files_of_type("png")

for (name, path) in files:
    embed_builder.add_embed(path)
    asset_name = name.upper() + "_IMAGE"
    file_enum_builder.add_entry(asset_name)

    json_path = path.replace("png", "json")
    if os.path.isfile(json_path):
        with open(json_path) as data:
            data = json.load(data)
            list = data["frames"]
            for entry in list:
                image_name = entry["filename"].upper().replace(" ", "_")
                image_name = image_name.replace("LAYER_1_", "")
                image_name = image_name.replace("__", "_")
                image_x = entry["frame"]["x"]
                image_y = entry["frame"]["y"]
                image_w = entry["frame"]["w"]
                image_h = entry["frame"]["h"]

                image = Image.open(path)
                aabb = image.crop((image_x, image_y, image_x + image_w, image_y + image_h)).getbbox()
                assert aabb is not None

                sprite_enum_builder.add_entry(image_name)
                transform_array_builder.add_entry("{%s, %s, %s, %s, %s, %s, %s, %s}"%(image_x,image_y,image_w,image_h,aabb[0],aabb[1],aabb[2],aabb[3]))
                sprite_to_image_array_builder.add_entry("ImageFile::%s"%(asset_name))
    else:
        sprite_enum_builder.add_entry(name.upper())
        image = Image.open(path)
        aabb = image.getbbox()
        assert aabb is not None
        transform_array_builder.add_entry("{%s, %s, %s, %s, %s, %s, %s, %s}"%(0, 0, image.size[0], image.size[1],aabb[0],aabb[1],aabb[2],aabb[3]))
        sprite_to_image_array_builder.add_entry("ImageFile::%s"%(asset_name))

sprite_enum_builder.add_entry("NONE")

text : str = build_header(
    [embed_builder, file_enum_builder, sprite_enum_builder, transform_array_builder, sprite_to_image_array_builder, f"constexpr int NUMBER_OF_IMAGES = {len(files)};"],
    ["image_structures.h"])

with open("../source/image_data.h", "w") as header:
    header.write(text)
