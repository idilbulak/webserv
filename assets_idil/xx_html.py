import os

_1xx_dir = "1xx_html"
_2xx_dir = "2xx_html"
_3xx_dir = "3xx_html"
_4xx_dir = "4xx_html"
_5xx_dir = "5xx_html"
_1xx = {
	"100": "100 Continue",
	"101": "Switching Protocols"
}
_2xx = {
	"200": "200 OK",
	"201": "201 Created",
	"202": "202 Accepted",
	"203": "203 Non-Authoritative Information",
	"204": "204 No Content",
	"205": "205 Reset Content"
}
_3xx = {
	"300": "300 Multiple Choices",
	"301": "301 Moved Permanently",
	"302": "302 Found",
	"303": "303 See Other",
	"305": "305 Use Proxy",
	"306": "306 (Unused)",
	"307": "307 Temporary Redirect"
}
_4xx = {
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
	"429": "Too Many Requests",
	"500": "Internal Server Error",
	"501": "Not Implemented",
	"502": "Bad Gateway",
	"503": "Service Unavailable",
	"504": "Gateway Timeout",
	"505": "HTTP Version Not Supported"
}
_5xx = {
	"500": "Internal Server Error",
	"501": "Not Implemented",
	"502": "Bad Gateway",
	"503": "Service Unavailable",
	"504": "Gateway Timeout",
	"505": "HTTP Version Not Supported"
}

for code, message in _1xx.items():
	_1xx_file = os.path.join(_1xx_dir, f"{code}.html")

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

	with open(_1xx_file, "w") as f:
		f.write(x)

print(f"Created _1xx_html/*")

for code, message in _2xx.items():
	_2xx_file = os.path.join(_2xx_dir, f"{code}.html")

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

	with open(_2xx_file, "w") as f:
		f.write(x)

print(f"Created _2xx_html/*")

for code, message in _3xx.items():
	_3xx_file = os.path.join(_3xx_dir, f"{code}.html")

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

	with open(_3xx_file, "w") as f:
		f.write(x)

print(f"Created _3xx_html/*")

for code, message in _4xx.items():
	_4xx_file = os.path.join(_4xx_dir, f"{code}.html")

	x = f"""
	<!DOCTYPE html>
	<html>
	<head>
		<title>{code} - {message}</title>
		<style>
			body {{
				background-color: black;
			}}
			.container {{
				display: flex;
				flex-direction: column;
				align-items: center;
				justify-content: center;
				height: 100vh;
				color: white;
				padding-top: 20px;
			}}
			h1 {{
				font-size: 4em;
				margin: 0.5em;
			}}
			p {{
				font-size: 2em;
				margin: 0.5em;
			}}
			img {{
				max-width: 100%;
				height: auto;
			}}
		</style>
	</head>
	<body>
		<div class="container">
			<h1>{code} - {message}</h1>
			<p>The server encountered an client error while processing your request.</p>
			<iframe src="https://giphy.com/embed/GODSCQebffJzW" width="339" height="480" frameBorder="0" class="giphy-embed" allowFullScreen></iframe>
		</div>
	</body>
	</html>
	"""

	with open(_4xx_file, "w") as f:
		f.write(x)

print(f"Created _4xx_html/*")

for code, message in _5xx.items():
	_5xx_file = os.path.join(_5xx_dir, f"{code}.html")

	x = f"""
	<!DOCTYPE html>
	<html>
	<head>
		<title>{code} - {message}</title>
		<style>
			body {{
				background-color: black;
			}}
			.container {{
				display: flex;
				flex-direction: column;
				align-items: center;
				justify-content: center;
				height: 100vh;
				color: white;
				padding-top: 20px;
			}}
			h1 {{
				font-size: 4em;
				margin: 0.5em;
			}}
			p {{
				font-size: 2em;
				margin: 0.5em;
			}}
			img {{
				max-width: 100%;
				height: auto;
			}}
		</style>
	</head>
	<body>
		<div class="container">
			<h1>{code} - {message}</h1>
			<p>The server encountered an server error while processing your request.</p>
			<iframe src="https://giphy.com/embed/GODSCQebffJzW" width="339" height="480" frameBorder="0" class="giphy-embed" allowFullScreen></iframe>
		</div>
	</body>
	</html>
	"""

	with open(_5xx_file, "w") as f:
		f.write(x)

print(f"Created _5xx_html/*")
