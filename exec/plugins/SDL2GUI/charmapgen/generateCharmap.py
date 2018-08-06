from PIL import Image, ImageFont, ImageDraw

im = Image.new("1", (128, 128))

draw = ImageDraw.Draw(im)
font = ImageFont.truetype("C64_Pro_Mono-STYLE.ttf", 8) #Courtesy of http://style64.org/

charlist = "".join([unichr(0x25CF) for x in range(32)] + [chr(x) for x in range(32, 127)] + [unichr(0x25CF) for x in range(129)]) #fill in blanks with a "null" char

for x in range(16): #16 lines of 16
    draw.text((0, 8*x), charlist[x*16:(x+1)*16], 1, font)

im.save("../charmap.bmp", "BMP")
