from PIL import Image, ImageFont, ImageDraw

font_path = "C:/Projects/ILATSPIDK/assets/atkinson_hyperlegible.ttf"
font = ImageFont.truetype(font_path, 128)

number_of_english_characters = 95
character_width = 128
character_height = 200
# LA    1
image = Image.new('LA', (character_width * number_of_english_characters, character_height))
draw = ImageDraw.Draw(image)
for i in range(number_of_english_characters):
    draw.text((i * character_width, 0), chr(32 + i), font=font, fill="white")

output_path = "C:/Projects/ILATSPIDK/assets/atlas/font.png"
image.save(output_path)
