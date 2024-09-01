import os
import inspect
import sys
import ast

def is_python_file(file):
    return file.endswith('.py')

def is_dangerous_code(code):
    dangerous = ('exec', 'eval', 'compile')
    for danger in dangerous:
        if danger in code:
            return True
    return False

def get_object_parameter_list(file_lines, cls_name):
    object_list = list()
    for line in file_lines:
        if cls_name + '(' in line and 'class' not in line and ':' not in line: # if there's an instantiation of object
            start_index = line.find('(')
            param_string = line[start_index:].strip()
            try:
                params = ast.literal_eval(param_string) # get the parameters
                object_list.append(params)
            except (SyntaxError, ValueError):
                print(f"Skipping line due to invalid parameter format: {line}")
    return object_list

def get_method_parameters(file_lines, method_name):
    for line in file_lines:
        if method_name + '(' in line: # if there's an execution of method
            start_index = line.find('(')
            if not line.find('def') < line.find(method_name): # if it's not a function declaration
                param_string = line[start_index:].strip()
                try:
                    params = ast.literal_eval(param_string) # get the parameters
                    return params
                except (SyntaxError, ValueError):
                    print(f"Skipping line due to invalid parameter format: {line}")

def decorate_method(foo, code):
    def ret(*args, **kwargs):
        local_context = {'args': args, 'kwargs': kwargs} # for capturing needed local variables
        exec(code, {}, local_context) # Execute the added code
        return foo(*args, **kwargs) # return the function with correct representation
    return ret

if __name__ == '__main__':

    file_name = input("Please enter python file name: ")
    while not is_python_file(file_name):
        print("Not a python file! Try again.")
        file_name = input("Please enter python file name: ")

    # check if the added code is safe
    added_code = input("Please enter added python code: ")
    while is_dangerous_code(added_code):
        print("Entered a dangerous code! Please try again.")
        added_code = input("Please enter added python code: ")

    if os.path.isfile(file_name):
        f = open(file_name, 'r')
        if f.readable():
            module = __import__(file_name[:-3]) # remove .py to get the module name

            # class list for the module
            classes = [cls_name for cls_name, cls_obj in
                       inspect.getmembers(sys.modules[module.__name__]) if inspect.isclass(cls_obj)]
            if not classes:
                print("No classes found!")
                exit()

            # get the class object
            cl = module.__dict__.get(classes[0])

            # get the function list for the class
            func_list = [method for method in dir(cl) if callable(getattr(cl, method))
                   and (not method.startswith("__") or (type(getattr(cl, method)) != type(getattr(object, method))))]

            #change the function and add the code
            for func in func_list:
                setattr(cl, func, decorate_method(getattr(cl, func), added_code))

            # get the lines of the file
            file_lines = f.readlines()

            # get the parameter list for the objects created in the module from the class
            obj_parameter_list = get_object_parameter_list(file_lines, classes[0])
            # create the objects with the parameters
            create_obj = [cl(*parameters) for parameters in obj_parameter_list]

            # run the functions of the object with the added code
            # it runs the functions only for one example
            # takes the example parameters from the given python file
            for obj in create_obj:
                for func in func_list:
                        if func != '__init__' and func != '__str__':
                            method_parameters = get_method_parameters(file_lines, func)
                            if method_parameters: # if there are parameters for the object
                                getattr(obj, func)(method_parameters)
                                print(obj)
                            else:
                                getattr(obj, func)()
                                print(obj)

        f.close()
    else:
        print(f'file "{file_name}" does not exist!')