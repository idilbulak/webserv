#!/usr/bin/python

import cgi
import os

def incrementcount():
    count_file_path = os.path.join(os.getcwd(), 'www/count.txt')
    try:
        with open(count_file_path, 'r+') as f:
            i = int(f.read().strip())
            i += 1
            f.seek(0)
            f.write(str(i))
            return i
    except Exception as e:
        return -1

print('Status: 200')
print('Content-type: text/html')
print("\r\n\r\n")
print('<html>')
print('<body>')
print('<h1>The current count is: {}</h1>'.format(incrementcount()))
print('<p><a href="/">Click here to return to the homepage</a></p>')
print('</body>')
print('</html>')
