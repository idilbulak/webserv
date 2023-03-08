#!/usr/bin/python3

import os

# Set the content type
print("Content-type: text/html\r\n")

# Get the query string
query_string = os.environ.get("QUERY_STRING", "")

# Parse the query string
params = {}
for param in query_string.split("&"):
    key, value = param.split("=")
    params[key] = value

# Generate the HTML response
html = """
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Hello from backend.cgi</title>
</head>
<body>
    <h1>Hello from backend.cgi</h1>
    <p>The following parameters were passed in the query string:</p>
    <ul>
"""

for key, value in params.items():
    html += f"<li>{key}={value}</li>"

html += """
    </ul>
</body>
</html>
"""

print(html)
