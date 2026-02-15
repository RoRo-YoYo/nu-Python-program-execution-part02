#
# input, int, float
#
print("Starting")

x = 1
y = 10.5
z = "fruit: "
print(x)
print(y)
print(z)

s = input("enter a number> ")
print()
y = int(s)
y = 2 * y

print(y)
print(s)

s2 = input("another number> ")
print()
x = float(s2)
x = x ** 2.0

print(x)
print(s2)

z2 = input("enter whatever you want> ")
print()
z = z + z2

print(z)

print("Done")
