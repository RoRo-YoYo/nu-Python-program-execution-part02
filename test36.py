#
# loop --- semantic error
#
print("starting")
print()

i = 100
print(i)

while i != 100:
{
    print("you should not see this")
    j = 123
}  

#
# the loop never executes so j is not defined above:
#
print(j)

print()
print("done")

