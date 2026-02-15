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
test4 = xyz / 3.0    ## error: undefined
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

print()
print('done')


