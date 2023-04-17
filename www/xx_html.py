import os

# Python dictionary(like hash table) named dirs is defined, contains sub-dictionaries.
# The keys of the sub-dictionaries are the status codes, and the values are the messages.
dirs = {
    "1xx_html": {
        "100": "100 Continue",
        "101": "Switching Protocols"
    },
    "2xx_html": {
        "200": "200 OK",
        "201": "201 Created",
        "202": "202 Accepted",
        "203": "203 Non-Authoritative Information",
        "204": "204 No Content",
        "205": "205 Reset Content"
    },
    "3xx_html": {
        "300": "300 Multiple Choices",
        "301": "301 Moved Permanently",
        "302": "302 Found",
        "303": "303 See Other",
        "305": "305 Use Proxy",
        "306": "306 (Unused)",
        "307": "307 Temporary Redirect"
    },
    "4xx_html": {
        "400": "Bad Request",
        "401": "Unauthorized",
        "402": "Payment Required",
        "403": "Forbidden",
        "404": "Not Found",
        "405": "Method Not Allowed",
        "408": "Request Timeout",
        "409": "Conflict",
        "410": "Gone",
        "413": "Payload Too Large",
        "414": "URI Too Long",
        "415": "Unsupported Media Type",
        "429": "Too Many Requests"
    },
    "5xx_html": {
        "500": "Internal Server Error",
        "501": "Not Implemented",
        "502": "Bad Gateway",
        "503": "Service Unavailable",
        "504": "Gateway Timeout",
        "505": "HTTP Version Not Supported"
    }
}

# Loop through each directory and its codes
for directory, codes in dirs.items():
    # Loop through each code and its message
    for code, message in codes.items():
        # Create a file path for the code
        file_path = "www/" + os.path.join(directory, f"{code}.html")

        # Create an HTML string with the code and message
        x = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>{code} {message}</title>
        </head>
        <body>
            <h1>{code}</h1>
            <p>{message}</p>
        </body>
        </html>
        """

        # Write the HTML string to a file
        with open(file_path, "w") as f:
            f.write(x)
            
    # Print a message to indicate that the files have been created
    # print(f"Created {directory}/*")
