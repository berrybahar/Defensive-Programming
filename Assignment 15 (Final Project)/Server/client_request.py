import base64
import binascii
import secrets
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Random import get_random_bytes
from crc import memcrc

cip = []

def decode_hex_string(hex_string):
    hex_string = hex_string.rstrip('0')
    try:
        decoded_string = bytes.fromhex(hex_string).decode('utf-8')
    except ValueError:
        return "Invalid hex string"

    return decoded_string.strip('\x00')

def create_client_id():
    random_bytes = secrets.token_bytes(16)
    random_hex = random_bytes.hex()
    return random_hex

def hex_string_to_int(hex_string):
    # Ensure the hex string length is even
    if len(hex_string) % 2 != 0:
        raise ValueError("Hex string length must be even.")

    # Convert the hex string to bytes
    byte_data = bytes.fromhex(hex_string)

    # Reverse the byte order for little-endian to big-endian conversion
    byte_data_reversed = byte_data[::-1]

    # Convert the reversed byte array to an integer
    integer_value = int.from_bytes(byte_data_reversed, byteorder='big')

    return integer_value


def int_to_hex_string(value, num_bytes):
    if value < 0:
        raise ValueError("Integer value must be non-negative.")

    # Ensure the value fits in the specified number of bytes
    max_value = (1 << (num_bytes * 8)) - 1
    if value > max_value:
        raise ValueError(f"Value {value} is too large to fit in {num_bytes} bytes.")

    # Convert the integer to bytes (little-endian format)
    byte_data = value.to_bytes(num_bytes, byteorder='little')

    # Convert bytes to hex string
    hex_string = byte_data.hex()

    return hex_string

def create_aes_key(public_key):
    pub = binascii.unhexlify(public_key)
    pubkey = RSA.importKey(pub)

    aes_key = get_random_bytes(32)  # 256 bits
    iv = bytes(16)
    cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    cipher_rsa = PKCS1_OAEP.new(pubkey)
    encrypted_aes_key = (cipher_rsa.encrypt(aes_key)).hex()

    return encrypted_aes_key, cipher

HOST_VERSION = int_to_hex_string(3, 1)

REGISTRATION = 825
RECCONECTION = 827
PROPER_CRC = 900
NONPROPER_CRC = 901
FOURTH_NONPROPER_CRC = 902

SEND_PUBLIC_KEY = 826
SEND_FILE = 828

SUCCESSFUL_REGISTRATION = int_to_hex_string(1600, 2)
FAILED_REGISTRATION = int_to_hex_string(1601, 2)
RECEIVED_PUBLIC_KEY_SEND_AES_KEY = int_to_hex_string(1602, 2)
RECEIVED_FILE_WITH_PROPER_CRC = int_to_hex_string(1603, 2)
RECEIVE_MESSAGE_CONFIRM = int_to_hex_string(1604, 2)
CONFIRM_RECONNECTION_SEND_AES_KEY = int_to_hex_string(1605, 2)
FAILED_RECONNECTION = int_to_hex_string(1606, 2)
FAILED_REQUEST = int_to_hex_string(1607, 2)

class ClientRequest:
    def __init__(self, client_request):
        self._client_ID = client_request[0:32]
        self._version = client_request[32:34]
        self._code = client_request[34:38]
        self._payload_size = client_request[38:46]
        self._payload = client_request[46:]

    def get_client_id(self):
        return self._client_ID
    def get_version(self):
        return self._version
    def get_code(self):
        return self._code
    def get_payload_size(self):
        return self._payload_size
    def get_payload(self):
        return self._payload

    def __str__(self):
        return self._client_ID + self._version + self._code + self._payload_size + self._payload

def registration(client_request, clients):
    client_name = decode_hex_string(client_request.get_payload())
    client_id = create_client_id()
    if client_name in clients:
        print('\nRegistration request failed.')
        return HOST_VERSION + FAILED_REGISTRATION + int_to_hex_string(0, 4)
    try:
        clients[client_name] = client_id
    except MemoryError:
        print('\nRequest failed.')
        return HOST_VERSION + FAILED_REQUEST + int_to_hex_string(0, 4)

    print('\nSuccessfully registered.')
    return HOST_VERSION + SUCCESSFUL_REGISTRATION + int_to_hex_string(len(client_id), 4) + client_id

def recconection(client_request, clients, client_pub_key):
    client_name = decode_hex_string(client_request.get_payload())
    if client_name not in clients.keys():
        print('\nReconection request failed.')
        return HOST_VERSION + FAILED_RECONNECTION + int_to_hex_string(0, 4), 0

    client_id = clients[client_name]
    public_key = client_pub_key[client_id]

    cipher_hex, cipher = create_aes_key(public_key)

    print('\nSuccessfully recconected.')
    return HOST_VERSION + CONFIRM_RECONNECTION_SEND_AES_KEY + int_to_hex_string(len(client_id + cipher_hex), 4) + client_id + cipher_hex, cipher

def proper_crc(client_request):
    file_name = decode_hex_string(client_request.get_payload())
    print('\nThe crc of the file is proper.')
    return HOST_VERSION + RECEIVE_MESSAGE_CONFIRM + int_to_hex_string(len(file_name.encode('utf-8').hex()), 4) + client_request.get_payload()

def nonproper_crc(client_request):
    print('\nThe crc of the file isn\'t proper.')
    return HOST_VERSION + FAILED_REQUEST + int_to_hex_string(0, 4)

def fourth_nonproper_crc(client_request):
    file_name = decode_hex_string(client_request.get_payload())
    print('\nThe crc of the file isn\'t proper for the fourth time.')
    return HOST_VERSION + RECEIVE_MESSAGE_CONFIRM + int_to_hex_string(len(file_name.encode('utf-8').hex()),4) + client_request.get_payload()

def send_public_key(client_request, clients, client_pub_key):
    payload = client_request.get_payload()
    client_name = decode_hex_string(payload[:510])
    client_id = clients[client_name]
    public_key = payload[510:]
    client_pub_key[client_id] = public_key

    cipher_hex, cipher = create_aes_key(public_key)

    print('\nReceived the public key and sending the aes key.')
    return HOST_VERSION + RECEIVED_PUBLIC_KEY_SEND_AES_KEY + int_to_hex_string(len(client_id + cipher_hex), 4) + client_id + cipher_hex, cipher

def send_file(client_request, cipher, files_list):
    # don't forget to save the file
    client_id = (client_request.get_payload())[:32]
    file_name = (client_request.get_payload())[24:534]
    file_size = int.from_bytes(bytes.fromhex((client_request.get_payload())[8:16]), byteorder='little')
    file_size_after_encryption = (client_request.get_payload())[:8]
    message_content = bytes.fromhex((client_request.get_payload())[534:])
    decrypted_file = cipher.decrypt(message_content)

    try:
        file = open(decode_hex_string(file_name), "wb")
        file.write(decrypted_file[:file_size])
        files_list.append(decode_hex_string(file_name))
        file.close()
    except IOError as e:
        print(f"\nAn error occurred while writing to the file: {e}")


    temp_crc = memcrc(decrypted_file[:file_size])
    num_bytes = (temp_crc.bit_length() + 7) // 8
    crc = (temp_crc.to_bytes(num_bytes, byteorder='little')).hex()

    payload = client_id + file_size_after_encryption + file_name + crc

    print('\nThe file is saved successfully.')
    return HOST_VERSION + RECEIVED_FILE_WITH_PROPER_CRC + int_to_hex_string(len(payload), 4) + payload


def answer(client_request, files_list, clients, client_pub_key):
    ans = ''
    if hex_string_to_int(client_request.get_code()) == REGISTRATION:
        client_name = decode_hex_string(client_request.get_payload())
        print('\nGot request for registration from client:', client_name)
        return registration(client_request, clients)
    elif hex_string_to_int(client_request.get_code()) == RECCONECTION:
        client_name = decode_hex_string(client_request.get_payload())
        print('\nGot request for recconection.')
        ans, cipher = recconection(client_request, clients, client_pub_key)
        if cipher != 0:
            cip.append(cipher)
        return ans
    elif hex_string_to_int(client_request.get_code()) == PROPER_CRC:
        print('\nCRC is alright.')
        return proper_crc(client_request)
    elif hex_string_to_int(client_request.get_code()) == NONPROPER_CRC:
        print('\nCRC isn\'t alright.')
        return nonproper_crc(client_request)
    elif hex_string_to_int(client_request.get_code()) == FOURTH_NONPROPER_CRC:
        print('\nCRC isn\'t alright for the fourth time.')
        return fourth_nonproper_crc(client_request)
    elif hex_string_to_int(client_request.get_code()) == SEND_PUBLIC_KEY:
        print('\nGot the public key.')
        ans, cipher = send_public_key(client_request, clients, client_pub_key)
        cip.append(cipher)
        return ans
    elif hex_string_to_int(client_request.get_code()) == SEND_FILE:
        print('\nGot request for saving file')
        temp_cip = cip[0]
        cip.clear()
        return send_file(client_request, temp_cip, files_list)