import os
import binascii

SAVE_FILE = 100
RETRIEVE_FILE = 200
DELETE_FILE = 201
GET_CLIENT_FILE_LIST = 202

class Request:
    def __init__(self, user_id, version, op, filename, payload):
        self.user_id = user_id.hex()
        self.version = version.to_bytes(1, byteorder='little', signed=False).hex()
        self.op = op.to_bytes(1, byteorder='little', signed=False).hex()

        if filename != '':
            self.filename = filename.replace('\0', '').encode('utf-8').hex()
        else:
            self.filename = ''

        if filename != '':
            self.name_len = len(filename).to_bytes(2, byteorder='little', signed=False).hex()
        else:
            self.name_len = ''

        if payload != '':
            self.payload = binascii.hexlify(payload).decode('utf-8')
        else:
            self.payload = ''

        if payload != '':
            self.size = os.path.getsize(filename).to_bytes(4, byteorder='little', signed=False).hex()
        else:
            self.size = ''

    def get_op(self):
        return self.op

    def get_file_name(self):
        return self.filename

    def __str__(self):
        if self.payload == '' and self.size == '' and self.filename == '' and self.name_len == '':
            return self.user_id + self.version + self.op
        elif self.payload == '' and self.size == '':
            return self.user_id + self.version + self.op + self.name_len + self.filename
        else:
            return self.user_id + self.version + self.op + self.name_len + self.filename + self.size + self.payload

def get_client_request_list(user_id, version, filename):
    try:
        file = open(filename, 'rb')
        payload = file.read()
        file.close()

        req_save_file = Request(user_id, version, SAVE_FILE, filename, payload)
        req_retrieve_file = Request(user_id, version, RETRIEVE_FILE, filename, '')
        req_delete_file = Request(user_id, version, DELETE_FILE, filename, '')
        req_get_client_file_list = Request(user_id, version, GET_CLIENT_FILE_LIST, '', '')

        client_request_list = [req_save_file, req_retrieve_file, req_delete_file, req_get_client_file_list]

        return client_request_list
    except FileNotFoundError:
        print("File not found")