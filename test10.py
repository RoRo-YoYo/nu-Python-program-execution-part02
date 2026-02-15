#
# operators with integers -- semantic error
#
x = 100        # 100
y = x - 140    # -40
z = y          # -40
a = 140 - z    # 180

print("x is:")
print(x)
print('y is:')
print(y)
print('z is:')
print(z)
print('a is:')
print(a)

z = y * y      # 1600
print("and now for z:")
print(z)

test1 = x + y    # 60
x = 1
test2 = x + y    # -39
y = 123
test3 = x * y    # 123
test4 = z / 3    # 533
test5 = z % 11   # 5
x = 4
test6 = test5 ** x   # 625

print("test variables:")
print(test1)
print(test2)
print(test3)
print(test4)
print(test5)
print(test6)

divisor = 3
x = 126 / divisor
y = 126 % divisorrr   ## error: undefined
print(x)
print(y)

print()
print('done')

