import socket
import selectors
import os
import struct
from client_request import ClientRequest, answer
from functools import partial

HOST = '127.0.0.1'
sel = selectors.DefaultSelector()
ans = ""

def receive_data(conn):
    try:
        # Receive the size of the data
        raw_size = conn.recv(4)
        if not raw_size:
            return None

        # Convert the size from network byte order to host byte order
        data_size = struct.unpack('!I', raw_size)[0]

        # Receive the actual data in chunks
        data = b''
        while len(data) < data_size:
            packet = conn.recv(data_size - len(data))
            if not packet:
                return None
            data += packet

        return data.decode('utf-8')
    except socket.error as e:
        print(f"Socket error: {e}")
        return None

def send_to_client(conn, data):
    try:
        data = data.encode('utf-8')

        # Determine data length and create a header
        data_length = len(data)
        header = struct.pack('!I', data_length)

        # Send the header first
        conn.sendall(header)

        # Send the actual data
        conn.sendall(data)
    except socket.error as e:
        print(f"Socket error: {e}")

def get_port():
    if not os.path.exists('port.info'):
        print('\nport.info not found! using port number: 1256.')
        return 1256
    else:
        try:
            f = open('port.info', 'r')
            line = f.readline()
            if line.isdigit():
                ret = int(line)
                f.close()
                return ret
        except:
            print('\nProblems with port.info using port number: 1256.')
        return 1256

def read(conn, mask, files_list, clients, client_pub_key):
    data = receive_data(conn)
    if data:
        client_request = ClientRequest(data)
        ans = answer(client_request, files_list, clients, client_pub_key)
        send_to_client(conn, ans)
    else:
        print('\nclosing', conn)
        sel.unregister(conn)
        conn.close()

def accept(sock, mask, files_list, clients, client_pub_key):
    conn, addr = sock.accept()
    print('\naccepted', conn, 'from', addr)
    conn.setblocking(False)
    sel.register(conn, selectors.EVENT_READ, partial(read, files_list=files_list, clients=clients, client_pub_key=client_pub_key))

def server():
    files_list = []
    clients = {}
    client_pub_key = {}

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    PORT = get_port()
    sock.bind((HOST, PORT))
    sock.listen()
    print("Server listening on port", PORT)
    sock.setblocking(False)
    sel.register(sock, selectors.EVENT_READ, partial(accept, files_list=files_list, clients=clients, client_pub_key=client_pub_key))
    try:
        while True:
            events = sel.select()
            for key, mask in events:
                callback = key.data
                callback(key.fileobj, mask)
    except KeyboardInterrupt:
        print('\nclosing server')
    finally:
        sel.close()

if __name__ == '__main__':
    server()