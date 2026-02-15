# nuPython Execution Engine – Part 2 (Project 07)

Extended the nuPython execution engine to support additional data types, operators, built-in functions, relational expressions, and control flow (if / while).

Only the following files were modified:
- main.c
- execute.c

---

## Scope of This Project

Project 07 adds support for:

- Boolean literals (True, False)
- Real numbers
- String operations
- Relational operators
- Additional built-in functions: input(), int(), float()
- if / elif / else statements
- while loops

Memory leak handling is not required.

---

## Boolean Support

Added support for:

- Boolean literals: `True`, `False`
- Stored as `RAM_TYPE_BOOLEAN`
  - False → 0
  - True → 1
- `print()` correctly outputs:
  - 0 → False
  - non-zero → True

---

## Extended Binary Operators

Extended `+`, `-`, `*`, `**`, `/`, `%` to support:

### Integer
- Integer result when both operands are integers

### Real
- Real result when both operands are reals
- Mixed int/real promotes to real

### String
- `+` performs string concatenation
- Dynamically allocates memory for result

### Semantic Errors
- Real divide by 0
- Real mod by 0 (uses `fmod()`)
- Invalid operand type combinations
- Undefined variables

Errors printed with `printf()` and execution stops.

---

## Relational Operators

Added support for:

- `==`
- `!=`
- `<`
- `<=`
- `>`
- `>=`

Rules:

- int vs int → compare as integers
- real vs real → compare as reals
- int vs real → compare as reals
- string vs string → compare using `strcmp()` (case-sensitive)
- Other combinations → semantic error

Result stored as `RAM_TYPE_BOOLEAN`:
- 0 → False
- 1 → True

---

## Additional Built-in Functions

### input("prompt")

- Prints prompt
- Reads full line from stdin
- Removes newline characters
- Stores result as string

### int(s)

- Converts string to integer using `atoi()`
- Special case: "0" must not be treated as failure
- On invalid conversion:
  - `**SEMANTIC ERROR: invalid string for int() (line ?)`

### float(s)

- Converts string to real using `atof()`
- Special case handling for "0"
- On invalid conversion:
  - `**SEMANTIC ERROR: invalid string for float() (line ?)`

---

## Required Helper Function

### execute_expr()

Implemented a required helper function that:

- Evaluates unary and binary expressions
- Returns computed result
- Detects semantic errors
- Is called by:
  - execute_assignment()
  - if statement execution
  - while loop execution

No global variables used.

---

## If Statements

Supported:

- if
- elif
- else

Execution model:

1. Evaluate condition using `execute_expr()`
2. Interpret result:
   - 0 → False
   - non-zero → True
3. Follow correct branch in program graph
4. Stop execution on semantic error

---

## While Loops

Execution model:

1. Evaluate condition using `execute_expr()`
2. If True:
   - Execute loop body
   - Re-evaluate condition
3. If False:
   - Exit loop
4. Stop execution on semantic error

---

## Constraints Enforced

- No function exceeds 100 lines (excluding comments)
- Required helper function `execute_expr()` defined and used
- No global variables
- All semantic errors stop execution immediately
- All functions contain header comments
