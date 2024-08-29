import types

class User:
    def __init__(self, name, occupation):
        self.name = name
        self.occupation = occupation

class Engineer(User):
    def __init__(self, name):
        super().__init__(name, "Engineer")

class ElectricalEngineer(Engineer):
    def __init__(self, name):
        super().super().__init__(name, "Electrical Engineer")

class ComputerEngineer(Engineer):
    def __init__(self, name):
        super().super().__init__(name, "Computer Engineer")

class MechanicalEngineer(Engineer):
    def __init__(self, name):
        super().super().__init__(name, "Mechanical Engineer")

class Technician(User):
    def __init__(self, name):
        super().__init__(name, "Technician")

class Politician(User):
    def __init__(self, name):
        super().__init__(name, "Politician")

def func():
    print()

if __name__ == "__main__":
    name_of_class = input("Please enter the name of new class: ")

    name_of_base_class = input("Please enter name of base class(blank if none): ")
    while (globals().get(name_of_base_class) is None) and (name_of_base_class != ''):
        print("Base class name doesn't exist. Please try again.")
        name_of_base_class = (input("Please enter name of base class(blank if none): "))

    name_of_method = input(f'Please enter name of new method for class {name_of_class}: ')
    name_of_attribute = input(f'Please enter name of new attribute for class {name_of_class}: ')

    if name_of_base_class != '': # if there is no base class
        added_class = type(name_of_class, (globals().get(name_of_base_class), ), {
            name_of_method: func(),
            name_of_attribute: 770
        })
    else:
        added_class = type(name_of_class, (), {
            name_of_method: func(),
            name_of_attribute: 770
        })

    print(f'Class {added_class.__name__} created with base class: {[word.__name__ for word in added_class.__bases__][0]}')
    print(f'Class __name__ is: {added_class.__name__}')
    print(f'Class __dict__ is: {added_class.__dict__}')