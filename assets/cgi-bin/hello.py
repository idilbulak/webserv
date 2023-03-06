#!/usr/bin/python3

import cgi

print("Content-type: text/html\n")
print("\r\n\r\n")
print("<html>")
print("<head><title>Visitor Name</title></head>")
print("<body>")
print("<h1>What is your name?</h1>")
print("<form method='post' action=''>")
print("First Name: <input type='text' name='first_name'><br>")
print("Last Name: <input type='text' name='last_name'><br>")
print("<input type='submit' value='Submit'>")
print("</form>")
print("</body>")
print("</html>")

