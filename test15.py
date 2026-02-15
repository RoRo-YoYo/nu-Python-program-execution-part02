#
# operators with reals -- semantic error
#
x = 100.234        
y = x - 140.9
z = y         
a = 1.5 - z    

print("x is:")
print(x)
print('y is:')
print(y)
print('z is:')
print(z)
print('a is:')
print(a)

z = y * y
print("and now for z:")
print(z)

test1 = x + y   
x = 1.2
test2 = x + y    
y = 123.123
test3 = x * y   
test4 = z / 3.0
test5 = z % 11.0
x = 4.2
test6 = test5 ** x

print("test variables:")
print(test1)
print(test2)
print(test3)
print(test4)
print(test5)
print(test6)

divisor = 3.0
x = 126.9 / divisor
y = 126.9 % divisor
print(x)
print(y)

#
# random comment
#

i = 100.2
j = 200.9
b1_1 = 100.2 < 200.9
b1_2 = i < 200.9
b1_3 = 100.2 < zebra       ## error: undefined
b1_4 = i < j

i = 123.0
j = 123.0
b2_1 = 123.0 != 123.0
b2_2 = i != 123.0
b2_3 = 123.0 != j
b2_4 = i != j

i = 90.5
j = 90.5
b3_1 = 90.5 >= 90.5
b3_2 = i >= 90.4
b3_3 = 90.4 >= j
b3_4 = i >= j

i = 123.625
j = 123.625
b4_1 = 123.625 == 123.625
b4_2 = i == 123.5
b4_3 = 123.5 == j
b4_4 = i == j

i = 100.5
j = 200.5
b5_1 = 100.5 > 200.5
b5_2 = i > 200.5
b5_3 = 100.5 > j
b5_4 = i > j

i = 200.5
j = 100.5
b6_1 = 200.5 > 100.5
b6_2 = i > 100.625
b6_3 = 200.625 > j
b6_4 = i > j

i = 123.123
j = 12.5
b7_1 = 123.123 != 123.5
b7_2 = i != 123.999
b7_3 = 12.5 != j
b7_4 = i != j
b7_5 = j != j

i = 101.25
j = 100.25
b8_1 = 101.25 <= 100.25
b8_2 = i <= 100.25
b8_3 = 100.5 <= j
b8_4 = i <= j
b8_5 = i <= i

i = 12.5
j = 123.5
b9_1 = 12.5 == 123.5
b9_2 = i == 123.5
b9_3 = 123.25 == j
b9_4 = i == j
b9_5 = j == j

