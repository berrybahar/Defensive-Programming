import socket
import os
import pathlib
import random
from request import get_client_request_list, SAVE_FILE, RETRIEVE_FILE, DELETE_FILE, GET_CLIENT_FILE_LIST
import struct
from server_answer import print_answer, ServerAnswer, get_file_list, hex_string_to_int, hex_string_to_filename

def get_host_and_port(server_info):
    server_info_file = open(server_info, "r")
    server_info = server_info_file.readline()
    server_info_file.close()
    host_and_port = server_info.split(":")
    return host_and_port[0], host_and_port[1]

def send_to_server(sock, data, small_threshold=1024, chunk_size=4096):
    # Encode the data to bytes if it's not already
    if isinstance(data, str):
        data = data.encode('utf-8')

    data_length = len(data)

    # Create a header with the length of the data in network byte order (big-endian)
    header = struct.pack('!I', data_length)

    try:
        # Send the header first
        sock.sendall(header)

        # Decide whether to send in chunks or in one go based on the size
        if data_length < small_threshold:
            # Small data: send all at once
            sock.sendall(data)
        else:
            # Large data: send in chunks
            for i in range(0, data_length, chunk_size):
                chunk = data[i:i + chunk_size]
                sock.sendall(chunk)

    except socket.error as e:
        print(f"Socket error occurred: {e}")

def receive_data(sock):
    # Receive the size of the data (4 bytes)
    raw_size = sock.recv(4)
    if not raw_size:
        return None

    # Convert size from network byte order to host byte order
    data_size = struct.unpack('!I', raw_size)[0]

    # Receive the actual data
    data = b''
    while len(data) < data_size:
        packet = sock.recv(data_size - len(data))
        if not packet:
            return None
        data += packet

    return data

def print_req(req):
    file_name = hex_string_to_filename(req.get_file_name())
    if hex_string_to_int(req.get_op()) == SAVE_FILE:
        print(f'\nSending save request to the server for file-{file_name}')
    elif hex_string_to_int(req.get_op()) == RETRIEVE_FILE:
        print(f'\nSending retrieve request to the server for file-{file_name}')
    elif hex_string_to_int(req.get_op()) == DELETE_FILE:
        print(f'\nSending delete request to the server for file-{file_name}')
    elif hex_string_to_int(req.get_op()) == GET_CLIENT_FILE_LIST:
        print('\nSending request to the server for file-list in the backup directory')

def main():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if not os.path.exists("server.info"):
                print("Server info does not exist!")
                exit()

            HOST, PORT = get_host_and_port("server.info")

            if not PORT.isalnum():
                print("Invalid port!")
                exit()

            print('Connecting to the server:', HOST)
            s.connect((HOST.strip(), int(PORT.strip())))
            print('Connected to the server!')

            max_four_byte_num = (2**32) - 1
            # Generate 4 bytes unsigned num
            random_bytes = random.randint(0, max_four_byte_num)
            # Convert bytes to an unsigned integer
            user_id = struct.pack('<I', random_bytes)

            path = pathlib.Path(__file__).parent.resolve()

            if not os.path.exists("backup.info"):
                print("Backup info does not exist!")
                exit()

            file_list = get_file_list('backup.info')

            # create request list for the files
            client_request_list_first_file = get_client_request_list(user_id, 1, file_list[0])
            client_request_list_second_file = get_client_request_list(user_id, 1, file_list[1])

            # create request list for the program
            total_request_list = [client_request_list_first_file[3], client_request_list_first_file[0],
                                  client_request_list_second_file[0], client_request_list_first_file[3],
                                  client_request_list_first_file[1], client_request_list_first_file[2],
                                  client_request_list_first_file[1]]

            i = 1
            while True:
                if i == 1:
                    # send the directory of python script
                    send_to_server(s, str(path))
                    i +=1
                    continue
                elif i == 2:
                    # send the id of user
                    send_to_server(s, str(random_bytes))
                    i +=1
                    continue

                # send the requests for the program
                if i < len(total_request_list) + 3:
                    print_req(total_request_list[i-3])
                    send_to_server(s, total_request_list[i-3].__str__())
                    # get here the response from the server
                    data = receive_data(s)
                    if hex_string_to_int(total_request_list[i-3].get_op()) == SAVE_FILE:
                        print_answer(ServerAnswer(data.decode('utf-8')),str(path), True)
                    else:
                        print_answer(ServerAnswer(data.decode('utf-8')),str(path), False)
                    i +=1
                else:
                    print("Closing the connection.")
                    send_to_server(s, "EXIT")
                    break
    except Exception as e:
        print(f'Could not open socket with {HOST}:{PORT}. Error: {e}')
    finally:
        s.close()
if __name__ == '__main__':
    main()