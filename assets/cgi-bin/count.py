#!/usr/bin/python3

import cgi
import os

def incrementcount():
    count_file_path = os.path.join(os.getcwd(), 'assets/cgi-bin/count.txt')
    try:
        with open(count_file_path, 'r+') as f:
            i = int(f.read().strip())
            i += 1
            f.seek(0)
            f.write(str(i))
            return i
    except Exception as e:
        return -1

print('Content-type: text/html')
print("\r\n\r\n")
print('<html>')
print('<body>')
print('<h1>The current count is: {}</h1>'.format(incrementcount()))
print('</body>')
print('</html>')
