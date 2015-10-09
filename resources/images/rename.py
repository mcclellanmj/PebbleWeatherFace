# Tiny script used to rename the files after they were split
import os
import re

for image in os.listdir("."):
    print("File is [%s]" % image)
    matches = re.search('-(\d)-(\d).png', image)

    if matches:
        row = int(matches.group(1))
        column = int(matches.group(2))
        index = (row * 5) + column
        os.rename(image, "%d.png" % index)
