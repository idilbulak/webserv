#!/usr/bin/python3

import cgi

# specify that we're returning HTML content
print("Content-Type: text/html")
# print a blank line to indicate the end of the header
print("\r\n\r\n")
# print the HTML content
print("<html>")
print("<head>")
print("<title>CGI Python Script</title>")
print("</head>")
print("<body>")
print("<h1>Cgi Python Script is successfully executed.</h1>")
print("</body>")
print("</html>")