import os

err_dir = "err_html"
err = {
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

for code, message in err.items():
	err_file = os.path.join(err_dir, f"{code}.html")

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
			<p>The server encountered an error while processing your request.</p>
			<iframe src="https://giphy.com/embed/GODSCQebffJzW" width="339" height="480" frameBorder="0" class="giphy-embed" allowFullScreen></iframe><p><a href="https://giphy.com/gifs/deal-with-it-sunglasses-snoop-dogg-GODSCQebffJzW">via GIPHY</a></p>
		</div>
	</body>
	</html>
	"""

	with open(err_file, "w") as f:
		f.write(x)

print(f"Created err_html/")
