FILE_RETRIEVED = 210
RETURN_CLIENT_FILE_LIST = 211
FILE_SAVED_OR_DELETED = 212
FILE_NOT_EXIST =  1001
CLIENT_NOT_HAVE_FILE = 1002
SERVER_PROBLEM = 1003

def get_file_list(backup_info):
    f_backup_info = open(backup_info, 'r')
    file_line = f_backup_info.readlines()
    file_list = [file.strip('\n') for file in file_line]
    f_backup_info.close()
    return file_list

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


def hex_string_to_filename(hex_string):
    # Ensure the hex string length is even
    if len(hex_string) % 2 != 0:
        raise ValueError("Hex string length must be even.")

    # Convert the hex string to bytes
    byte_data = bytes.fromhex(hex_string)

    # Decode the bytes to a string
    filename = byte_data.decode('utf-8')

    return filename

class ServerAnswer:
    def __init__(self, server_answer):
        self._status = hex_string_to_int(server_answer[2:6])
        if self._status != CLIENT_NOT_HAVE_FILE and self._status != SERVER_PROBLEM:
            name_len = hex_string_to_int(server_answer[6:10])
            self._filename = hex_string_to_filename(server_answer[10:10 + name_len*2])
        else:
            self._filename = ""

    def getstatus(self):
        return self._status
    def getfilename(self):
        return self._filename

def print_answer(answer, script_dir, save):
    if answer.getstatus() == FILE_RETRIEVED:
        path = script_dir + '\\' +'tmp'
        tmp = bytes.fromhex(answer.getfilename())
        file_name = tmp.decode('utf-8')
        print(f'\nRetrieved file-{file_name} in {path}')
    elif answer.getstatus() == RETURN_CLIENT_FILE_LIST:
        file_list = get_file_list(script_dir + '\\' + answer.getfilename())
        s = ''
        for file_name in file_list:
            s += file_name + ','
        s = s[:-1]
        print(f'\nFiles in the backup directory are: {s}')
    elif answer.getstatus() == FILE_SAVED_OR_DELETED:
        if save:
            print(f'\nFile-{answer.getfilename()} saved successfully!')
        else:
            print(f'\nFile-{answer.getfilename()} deleted successfully!')
    elif answer.getstatus() == FILE_NOT_EXIST:
        print(f'\nFile-{answer.getfilename()} does not exist!')
    elif answer.getstatus() == CLIENT_NOT_HAVE_FILE:
        print('\nDon\'t have any files in the backup directory!')
    elif answer.getstatus() == SERVER_PROBLEM:
        print('\nServer problem! Can not fulfill the wanted operation!')