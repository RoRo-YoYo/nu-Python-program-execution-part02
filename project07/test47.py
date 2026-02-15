#
# if stmts and while loops together
#
print("starting")
print("")

print('**first loop:')

x = 123

if x > 122:
{
  while x <= 125:
  {
    if x == 123:
    {
      print("x is 123")
      print(x)
    }
    elif x == 124:
    {
      print("x is 124")
      print(x)
    }
    else:
    {
      print("x is 125")
      print(x)
    }

    x = x + 1
    print(x)
  }
}
else:
{
  # should not happen:
  if True:
  {
    print("should not happen!")
  }
}

print(x)
print('after first if stmt')
print(x)

y = 0
z = 1  ## address of y
*z = 10

print()
print('**second loop:')

while y > 0:
{    
    print('outer loop:')
    print(y)
    y = y - 1

    if True:
    {
      print('inner loop:')

      while y > 5:
      {
          if False:
          {
            print("should not happen when false")
          }
          else:
          {
            print('inner else')
            print(y)
            y = y - 1
          }
      }
    }

    print('bottom outer')
    print(y)
    y = y - 1
}

print("")
print("done")
