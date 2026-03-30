from cpp_utils import *
from PIL import Image, ImageFont, ImageDraw

font_path = "C:/Projects/ILATSPIDK/assets/font/atkinson_hyperlegible.ttf"
english_start = 32
number_of_english_characters = 95
last_english_character = english_start + number_of_english_characters - 1

font_builder = ArrayBuilder("Font", "fonts")
width_builder = ArrayBuilder("float", "character_widths")

number_of_fonts = 0

width_map = []

def create_font(name : str, size : int):
    global font_path
    global number_of_english_characters
    global width_map
    width_map = []
    font = ImageFont.truetype(font_path, size)
    width : int = size
    height : int = round(width * 1.5)
    image = Image.new('LA', (width * number_of_english_characters, height))
    draw = ImageDraw.Draw(image)
    for i in range(number_of_english_characters):
        text = chr(english_start + i);
        width_map.append(draw.textlength(text, font=font))
        draw.text((i * width + 3, 0), text, font=font, fill="white") # Add a little spacing of 3 from the left
    output_path = f"C:/Projects/ILATSPIDK/assets/font/{name}_font.png"
    image.save(output_path)

    global font_builder
    font_builder.add_entry("{%s, %s, %s, %s}"%(f"ImageFile::{name.upper()}_FONT_IMAGE", size, width, height))

    global number_of_fonts
    number_of_fonts += 1

create_font("extra_small", 8)
create_font("small", 16)
create_font("small_medium", 20)
create_font("medium", 24)
create_font("medium_large", 36)
create_font("large", 48)
create_font("very_large", 72)

number_of_fonts_text = f"constexpr int NUMBER_OF_FONTS = {number_of_fonts};"
english_start_text = f"constexpr int ENGLISH_STARTING_CHARACTER = {english_start};"
for width in width_map: width_builder.add_entry(str(width))

from fontTools import agl # pip install fonttools
from external.getKerningPairsFromOTF import OTFKernReader # https://github.com/adobe-type-tools/kern-dump/blob/main/getKerningPairsFromOTF.py
kerning_data = {} # want a dictionary of dictionary of number
kern_reader = OTFKernReader(font_path)
#print(kern_reader.kerningPairs)
for pair in kern_reader.kerningPairs:
    first_glyph = agl.toUnicode(pair[0])
    if not english_start <= ord(first_glyph) <= last_english_character: continue
    second_glyph = agl.toUnicode(pair[1])
    if not english_start <= ord(second_glyph) <= last_english_character: continue
    first_index = ord(first_glyph) - english_start
    second_index = ord(second_glyph) - english_start
    if first_index not in kerning_data:
        kerning_data[first_index] = {}
    kerning_data[first_index][second_index] = kern_reader.kerningPairs[pair]
#print(kerning_data)

kerning_text = "constexpr i8 kerning[][%s] = {\n"%number_of_english_characters;
for i in range(number_of_english_characters):
    kerning_text += "\t {"
    for j in range(number_of_english_characters):
        if i in kerning_data and j in kerning_data[i]:
            kerning_text += f"{kerning_data[i][j]}, "
        else:
            kerning_text += "0, "
    kerning_text += "},\n"
kerning_text += "};"

text : str = build_header([font_builder, number_of_fonts_text, english_start_text, width_builder, kerning_text], ["font_structures.h"])

with open("../source/font_data.h", "w") as header:
    header.write(text)
