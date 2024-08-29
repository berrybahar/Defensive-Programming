def capitalize_and_print_words_with_o(s):
    if type(s) == str:
        centence_list = s.split(' ')
        o_list = list()
        for word in centence_list:
            if 'o' in word:
                o_list.append(word.capitalize())
        for i in range(len(o_list) - 1):
            print(o_list[i], end=", ")
        print(o_list[len(o_list) - 1])

if __name__ == '__main__':
    line = 'This line contains words and some have the letter o'
    capitalize_and_print_words_with_o(line)