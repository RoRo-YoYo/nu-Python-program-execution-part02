#
# If statements with semantic error:
#
print("STARTING")
print()

s = input('Enter an integer> ')
print()
x = int(s)

if x < 0:
{
  print("less than 0")
}
elif x > 0:
{
  print("greater than 0")
}
elif fred == 0:   ### we don't hit this semantic error
{
  print("equal to 0")
}
else:
{
  print('this should never happen (1)!')
}

if x <= 0:
{
  print("less than or equal to 0")
}
elif abc >= 0:  ### semantic error: abc doesn't exist
{
  print("greater than or equal to 0")
}
elif x != 0:
{
  print("not equal to 0")
}
else:
{
  print('this should never happen (2)!')
}

x = x + 1

print()
print("DONE")
