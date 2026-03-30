from cpp_utils import *

embed_builder = EmbedBuilder("audio")
file_enum_builder = EnumBuilder("AudioFile")

files = get_files_of_type("wav")

for (name, path) in files:
    embed_builder.add_embed(path)
    asset_name = name.upper() + "_AUDIO"
    file_enum_builder.add_entry(asset_name)

text : str = build_header([embed_builder, file_enum_builder, f"constexpr int NUMBER_OF_SOUNDS = {len(files)};"], [])

with open("../source/audio_data.h", "w") as header:
    header.write(text)
