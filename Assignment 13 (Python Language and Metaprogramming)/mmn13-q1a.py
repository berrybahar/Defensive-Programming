def list_of_b(a_list):
    if type(a_list) == list:
        b_list = list()
        for name in a_list:
            if name.startswith('b'):
                b_list.append(name.capitalize())
        return b_list

if __name__ == '__main__':
    a_list = ["apple", "banana", "carrot", "black", "box"]
    b_list = list_of_b(a_list)
    print(b_list)
