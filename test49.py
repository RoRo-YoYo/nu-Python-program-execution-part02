#
# test16.py
#
# ptr assignment with semantic error: invalid address
#
print("TEST CASE: test16.py")
print()

x = 1234    # x has address 0
y = 5678    # y has address 1
r = 144.75  # r has address 2
s = "I am a string"   # s has address 3
z = 1       # z has address 4  

ptr_x = 0
ptr_y = 1
ptr_r = 2
ptr_s = 3
ptr_z = 4

*ptr_x = 5
*ptr_y = "Changing type from int to string, gotta love Python"
*ptr_r = 3.5
*ptr_s = "This is a different string altogether, fun!"
*ptr_z = 3

print(x)
print(y)
print(r)
print(s)
print(z)

print()

ptr_xyz = 11       # address 10 is valid (ptr_xyz), so address 11 is not

*ptr_xyz = 123    # semantic error, invalid address

*ptr_x = x - z    # x = 5 - 3 => 2
*ptr_z = x ** 5   # z = 2^5   => 32

print(x)    # 2
print(z)    # 32

print()
print("DONE")
