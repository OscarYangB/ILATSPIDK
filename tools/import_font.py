from PIL import Image, ImageFont, ImageDraw

font_path = "C:/Projects/ILATSPIDK/assets/atkinson_hyperlegible.ttf"
number_of_english_characters = 95

def create_font(name : str, size, width, height):
    global font_path
    global number_of_english_characters
    font = ImageFont.truetype(font_path, size)
    image = Image.new('LA', (width * number_of_english_characters, height))
    draw = ImageDraw.Draw(image)
    for i in range(number_of_english_characters):
        draw.text((i * width, 0), chr(32 + i), font=font, fill="white")
    output_path = f"C:/Projects/ILATSPIDK/assets/atlas/{name}_font.png"
    image.save(output_path)

create_font("small", 16, 16, 32)
create_font("medium", 24, 24, 48)
create_font("large", 48, 48, 96)
#create_font("medium", 16, 16, 25)
#create_font("large", 32, 32, 50)
#create_font("large", 64, 64, 100)
