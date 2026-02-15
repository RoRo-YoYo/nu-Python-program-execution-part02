#
# int, real, string concat
#
print("starting")
print("")

x = 1
y = 10.5
z = "shorter"
x = 2

print(x)
print(y)
print(z)

a = z + " string"
print(a)

b = x + 3.675
c = y + 10
d = x + 1
some_var = "cs"

print(b)
print(c)
print(d)

e = z + "+a very long string of word that could be many many words --- did you dynamically allocate?"
print(e)

f = some_variable + " 211"      ## error: undefined
print(f)

x = 1
y = 10.5
z = "shorter"

a = 10
b = 3.675
c = "cs "
d = "a very long string of word that could be many many words --- did you dynamically allocate? "

e = "211"
print("")

var1 = x + a
var2 = b + y
var3 = c + e
var4 = d + z
var5 = b + x
var6 = a + y

x = x + x
b = b + b
e = e + e

print(var1)
print(var2)
print(var3)
print(var4)
print(var5)
print(var6)
print(x)
print(b)
print(e)

print("")
print("done")
