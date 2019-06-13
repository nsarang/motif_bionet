from http.server import SimpleHTTPRequestHandler, HTTPServer
from glob import iglob
import os

os.chdir(os.path.dirname(os.path.realpath(__file__)))

class MyHTTPRequestHandler(SimpleHTTPRequestHandler):
    def translate_path(self, path):
        path = SimpleHTTPRequestHandler.translate_path(self, path)
        if os.path.isdir(path):
            for file in iglob(path + "*.html"):
                if os.path.basename(file).startswith("Visualization"):
                    return file
                # for ext in ".html", ".htm", ".txt":
                #     index = path + "/" + base + ext
                #     if os.path.exists(index):
                #         return index
        return path

def run():
    print('starting server...')
    # Server settings
    # Choose port 8080, for port 80, which is normally used for a http server, you need root access
    server_address = ('', 9999)
    httpd = HTTPServer(server_address, MyHTTPRequestHandler)
    print('running server...')
    httpd.serve_forever()


run()

