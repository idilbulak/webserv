#!/usr/bin/python

import cgi

print('Status: 200')
# specify that we're returning HTML content
print("Content-type: text/html")
# print a blank line to indicate the end of the header
print("\r\n\r\n")
# print the HTML content
print("<html>")
print("<head>")
print("<title>CGI Python Script</title>")
print("</head>")
print("<body>")
print("<h1>Cgi Python Script is successfully executed.</h1>")
print('<p><a href="/">Click here to return to the homepage</a></p>')
print("</body>")
print("</html>")