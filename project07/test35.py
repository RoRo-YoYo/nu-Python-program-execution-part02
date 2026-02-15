#
# loop --- semantic error
#
print("starting")
print("")

s = "string"
i = 123

while s != "string???":
{
   s = s + "?"
   print(s)

   while i != 0:  # j is not defined
   {
     i = i + j
   }
   
   print(s)
}

print("")
print("done")
