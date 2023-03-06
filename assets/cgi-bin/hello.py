#!/usr/bin/python3

import cgi

print("Content-type: text/html\n")
print("\r\n\r\n")
print("<html>")
print("<head><title>upload</title></head>")
print("<body>")
print("<h1>New user:</h1>")
print("<form method='post' action=''>")
print("First Name: <input type='text' name='first_name'><br>")
print("Last Name: <input type='text' name='last_name'><br>")
print("<input type='submit' value='Add'>")
print("</form>")
print("</body>")
print("</html>")


