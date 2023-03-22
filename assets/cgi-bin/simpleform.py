#!/usr/bin/python

import os

print("Content-type: text/html")
print("\r\n\r\n")
print("<html>")
print("<body>")
print("<h1>" + os.environ["QUERY_STRING"] + "</h1>")
print("</body>")
print("</html>")
