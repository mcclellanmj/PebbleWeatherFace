import os
import re

for image in os.listdir("."):
    matches = re.search("(\d+).png", image)

    if matches:
        print("{")
        print('"file": "images/45x45/%s",' % image)
        print('"name": "WEATHER_ICON_%s",' % matches.group(1))
        print('"type": "png"')
        print("},")
