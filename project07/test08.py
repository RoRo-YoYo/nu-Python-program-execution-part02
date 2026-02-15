#
# operators with mix of ints and reals
#
x = 100      
y = x - 140.9
z = y         
a = 1 - z    

print("x is:")
print(x)
print('y is:')
print(y)
print('z is:')
print(z)
print('a is:')
print(a)

z = a * x
print("and now for z:")
print(z)

test1 = x + y   
x = 1
test2 = y + x  
y = 123.123
test3 = x * y   
test4 = z / 3
x = 1000
test5 = z % 11.1
x = 4
test6 = test5 ** x

print("test variables:")
print(test1)
print(test2)
print(test3)
print(test4)
print(test5)
print(test6)

divisor = 3
x = 126.5 / divisor
y = 126.5 % divisor
print(x)
print(y)

test007 = "this is a string, fun!"
test008 = test007
print(test008)

#
# random comment
#

i = 100
j = 200.9
b1_1 = 100 < 200.9
b1_2 = i < 200.9
b1_3 = 100 < j
b1_4 = i < j

i = 123.0
j = 123
b2_1 = 123.0 != 123
b2_2 = i != 123
b2_3 = 123.0 != j
b2_4 = i != j

i = 90.5
j = 90
b3_1 = 90.5 >= 90
b3_2 = i >= 90
b3_3 = 90.4 >= j
b3_4 = i >= j

i = 123.625
j = 124
b4_1 = 123.625 == 124
b4_2 = i == 124
b4_3 = 123.5 == j
b4_4 = i == j

i = 100.5
j = 200
b5_1 = 100.5 > 200
b5_2 = i > 200
b5_3 = 100.5 > j
b5_4 = i > j

i = 200
j = 100.5
b6_1 = 200 > 100.5
b6_2 = i > 100.625
b6_3 = 200 > j
b6_4 = i > j

i = 123.123
j = 12
b7_1 = 123.123 != 12
b7_2 = i != 12
b7_3 = 12.5 != j
b7_4 = i != j
b7_5 = j != j

i = 101.25
j = 100.25
b8_1 = 101 <= 100.25
b8_2 = i <= 100.25
b8_3 = 100 <= j
b8_4 = i <= j
b8_5 = i <= i

i = 12.5
j = 123
b9_1 = 12.5 == 123
b9_2 = i == 123
b9_3 = 123.25 == j
b9_4 = i == j
b9_5 = i == i

ptri = 14
*ptri = j * 10

ptri = 54
*ptri = 10 - 20
