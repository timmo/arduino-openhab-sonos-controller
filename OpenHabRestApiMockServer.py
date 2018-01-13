from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from SocketServer import ThreadingMixIn
import threading
import argparse
import re
import cgi

# Openhab fake items
class Items:
    SonosPresets = "RadioEins,Deutschlandfunk,Gute Musik,Blues"
    ROOM_SelectedSonosPreset =''

class HTTPRequestHandler(BaseHTTPRequestHandler):


    def do_GET(self):
        if None != re.search('/rest/items/SonosPresets/state', self.path):
            self.send_response(200)
            self.send_header('Content-Type', 'text/plain')
            self.send_header('Content-Length', len( Items.SonosPresets))
            self.end_headers()
            self.wfile.write(Items.SonosPresets)
        elif None != re.search('/rest/items/ROOM_SelectedSonosPreset/state', self.path):
            self.send_response(200)
            self.send_header('Content-Type', 'text/plain')
            self.send_header('Content-Length', len( Items.ROOM_SelectedSonosPreset))
            self.end_headers()
            self.wfile.write(Items.ROOM_SelectedSonosPreset)
        else:
            self.send_response(404)
            self.end_headers()
        return

    def do_PUT(self):
        if None != re.search('/rest/items/ROOM_SelectedSonosPreset/state', self.path):
            contentType = cgi.parse_header(self.headers.getheader('content-type'))
            length = int(self.headers.getheader('content-length'))
            Items.ROOM_SelectedSonosPreset = self.rfile.read(length)
            self.send_response(200)
            self.end_headers()
            print 'ROOM_SelectedSonosPreset "%s"' % Items.ROOM_SelectedSonosPreset                
        else:
            self.send_response(404)
            self.end_headers()
        return

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
  allow_reuse_address = True

  def shutdown(self):
    self.socket.close()
    HTTPServer.shutdown(self)

class SimpleHttpServer():
  def __init__(self, ip, port):
    self.server = ThreadedHTTPServer((ip,port), HTTPRequestHandler)

  def start(self):
    self.server_thread = threading.Thread(target=self.server.serve_forever)
    self.server_thread.daemon = True
    self.server_thread.start()

  def waitForThread(self):
    self.server_thread.join()

  def stop(self):
    self.server.shutdown()
    self.waitForThread()

if __name__=='__main__':
    server = SimpleHttpServer("10.49.8.193", 8080)
    print 'HTTP Server Running...........'
    server.start()
    server.waitForThread()

