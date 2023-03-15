#!/usr/bin/python3

import cgi
import cgitb
import os

cgitb.enable()

print("Content-type: text/html")
print("\r\n\r\n")

# Check if the form has been submitted
form = cgi.FieldStorage()
if "file" not in form:
    print("<h1>No file was uploaded.</h1>")
else:
    # Get the file data from the form
    fileitem = form["file"]
    
    # Check if the file was uploaded
    if fileitem.filename:
        # Create a new file in the uploads directory
        filepath = os.path.join("uploadDir", os.path.basename(fileitem.filename))
        with open(filepath, "wb") as f:
            # Write the file data to the new file
            f.write(fileitem.file.read())
        print("<h1>File uploaded successfully.</h1>")
    else:
        print("<h1>No file was uploaded.</h1>")
