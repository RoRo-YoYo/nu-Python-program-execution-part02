#
# int, real, string concat -- semantic error
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

f = some_var + " 211"
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

var1 = True
var2 = False
var3 = var1 + var2    ## error: types

print(var1)
print(var2)
print(var3)

print("")
print("done")
