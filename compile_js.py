import os

def process_javascript():
  output_directory = 'build/generated/js/'
  output_file = 'weather_request.js'

  if not os.path.exists(output_directory):
    os.makedirs(output_directory)

  with open('src/weather_request.js') as infile, open(output_directory + output_file, 'w') as outfile:
    for line in infile:
      modified_line = line.replace('%WEATHER_UNDERGROUND_KEY%', os.environ['WEATHER_UNDERGROUND_KEY'])
      outfile.write(modified_line)