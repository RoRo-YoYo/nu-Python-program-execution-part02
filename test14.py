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
y = 126 % divisor
print(x)
print(y)

#
# random comment
#

i = 100
j = 200
b1_1 = 100 < 200
b1_2 = i < 200
b1_3 = 100 < j
b1_4 = i < j

i = 123
j = 123
b2_1 = 123 != 123
b2_2 = i != 123
b2_3 = 123 != j
b2_4 = i != j

i = 90
j = 90
b3_1 = 90 >= 90
b3_2 = i >= 90
b3_3 = 90 >= j
b3_4 = i >= j

i = 123
j = 123
b4_1 = 123 == 123
b4_2 = i == 123
b4_3 = 123 == j
b4_4 = i == j

i = 100
j = 200
b5_1 = 100 > 200
b5_2 = i > 200
b5_3 = 100 > j
b5_4 = i > j

i = 200
j = 100
b6_1 = 200 > 100
b6_2 = i > 100
b6_3 = 200 > j
b6_4 = i > j

i = 123
j = 12
b7_1 = 123 != 12
b7_2 = i != 12
b7_3 = 123 != j
b7_4 = i != j
b7_5 = i != i

i = 101
j = 100
b8_1 = 101 <= 100
b8_2 = abc <= 100             ## error: undefined
b8_3 = 101 <= j
b8_4 = i <= j
b8_5 = i <= i

i = 12
j = 123
b9_1 = 12 == 123
b9_2 = i == 123
b9_3 = 12 == j
b9_4 = i == j
b9_5 = j == j

