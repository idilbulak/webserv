#!/usr/bin/python3

import cgi

# Get the uploaded file from the request
form = cgi.FieldStorage()
file = form['file']

# Save the uploaded file to disk
filename = file.filename
with open(filename, 'wb') as f:
    f.write(file.file.read())

# Generate a response
print("Content-type: text/plain\n\n")
print("File uploaded successfully: {}".format(filename))

