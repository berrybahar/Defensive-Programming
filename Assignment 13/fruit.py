class AppleBasket:
    def __init__(self, color, count):
        self.apple_color = color
        self.apple_quantity = count

    def increase(self):
        self.apple_quantity += 1

    def __str__(self):
        return f'A basket of {self.apple_quantity} {self.apple_color} apples.'

class GreenAppleBasket(AppleBasket):
    def __init__(self, count):
        super().__init__("Green", count)

if __name__ == '__main__':
    apple1 = AppleBasket('red', 3)
    apple2 = AppleBasket('blue', 49)
    apple1.increase()
    print(apple1)
    apple2.increase()
    print(apple2)