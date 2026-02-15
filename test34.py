#
# loops --- semantic error
#
print("starting")
print("")

s = "string"
var = 123

while var != "string???":    # type error
{
   s = s + "?"
   print(var)
}

print("")
print("done")
