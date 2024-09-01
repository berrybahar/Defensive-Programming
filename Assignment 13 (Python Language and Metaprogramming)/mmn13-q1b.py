if __name__ == '__main__':
    a_list = ["apple", "banana", "carrot", "black", "box"]
    b_list = [word.capitalize() for word in a_list if word.startswith('b')]
    print(b_list)