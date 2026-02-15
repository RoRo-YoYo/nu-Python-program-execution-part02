#
# input, int, float --- with invalid numeric input
#
print("Starting")

x = 3
y = 10.5
z = " END"
print(x)
print(y)
print(z)

z2 = input("enter some text> ")
print()
z = z2 + z

print(z)

s = input("enter a number> ")
print()
y = int(s)
y = 2 * y

print(y)
print(s)

s = input("enter a number> ")
print()
y = int(s)
y = y + x

print(y)
print(s)

s2 = input("another number> ")
print()
x = float(s2)
x = x ** 2.0

print(x)
print(s2)

print("Done")
