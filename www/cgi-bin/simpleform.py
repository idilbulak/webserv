#!/usr/bin/python

import os

print('Status: 200')
print("Content-type: text/html")
print("\r\n\r\n")
print("<html>")
print("<body>")
print("<h1>" + os.environ["QUERY_STRING"] + "</h1>")
print('<p><a href="/">Click here to return to the homepage</a></p>')
print("</body>")
print("</html>")
