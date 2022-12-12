import threading

is_connected_to_server:bool = False

thread_for_incoming_msg:threading.Thread

from http.server import HTTPServer, CGIHTTPRequestHandler
server_address = ("", 8000)
httpd = HTTPServer(server_address, CGIHTTPRequestHandler)
httpd.serve_forever()
