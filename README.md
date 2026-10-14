# Tack
Tiny Assembly C Kernel, a stack-based, minimal, assembly-like language. Built for no better reason than to sharpen my programming skills, to explore VM/Stack machines, language design, and assembly-like languages.

---
## Instructions

| **Instruction** | **Name**              | **Purpose**                                                                                                                           |
| --------------- | --------------------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| push            | Push                  | Pushes a value to the stack. Takes an integer or character. Stack depth is limited (256 by default). Pushing past limit causes error. |
| pop             | Pop                   | Pops the last value from stack. Popping an empty stack causes error.                                                                  |
| add             | Add                   | Pops the last two values, adds them, pushes the result back.                                                                          |
| sub             | Subtract              | Same as add, but for subtracting.                                                                                                     |
| mul             | Multiply              | you can guess...                                                                                                                      |
| div             | Divide                | Pops last two values, divides the second last value with last, and pushes the result back. Division by zero causes error.             |
| mod             | Modulo                | Same as `div` but pushes the remainder instead.                                                                                       |
| gt              | Greater than          | Pops last two values, pushes `1` if second last is greater than last, otherwise `0`.                                                  |
| lt              | Less than             | Same as `gt` but pushes `1` if second last is less than last.                                                                         |
| eq              | Equal                 | Pushes `1` if both are equal.                                                                                                         |
| ne              | Not equal             | Pushes `1` if they are not equal.                                                                                                     |
| ge              | Greater than or Equal | Combination of `gt` and `eq`.                                                                                                         |
| le              | Less than or Equal    | Combination of `lt` and `eq`.                                                                                                         |
| jmp             | Jump                  | Jumps to the provided label.                                                                                                          |
| jmpz            | Jump if Zero          | Jumps to the provided label ONLY if stack top is `0`. Pops the stack top after execution ONLY if it jumped.                                 |
| printn          | Print number          | Pops and prints the stack head to the `stdout` as a number.                                                                           |
| printc          | Print character       | Pops and prints the stack head as a character to `stdout`.                                                                            |
| readn           | Read number           | Reads a number from `stdin` and pushes it to the stack. Blocks for input. EOF is considered as `0`.                                   |
| readc           | Read character        | Reads a character from `stdin` and pushes it's ASCII value to the stack. Blocks. EOF is `0` (NUL).                                    |
| dup             | Duplicate             | Duplicates the last value.                                                                                                            |
| over            | Over                  | Duplicates the second last value.                                                                                                     |
| swap            | Swap                  | Swaps the last two values.                                                                                                            |

---
## Labels

Just like labels in assembly, `TACK` also has labels to name sections of your code that you can jump to. They are of the form:

```tack
label_name:
```

Nothing can be written on the same line after the colon (except for comments and whitespace). Labels can't share the same name. They can only be declared at the start of a line. You can jump to them from anywhere in the code.

---
## Extra Stuff

You have access to single line comments starting with a `#` symbol just like python. Everything after the `#` is ignored untill the next line. Some examples are:
```tack
# Hello World
push 10 # This pushes 10
printn
# Output: 10
```

---
## CLI Options
I have also added some CLI flags/options for... Well, for convenience. All of them are listed below.

| Option | Purpose                                                                               |
| ------ | ------------------------------------------------------------------------------------- |
| -v     | Verbose. Keeps telling you what its doing like that one friend that never shuts up.   |
| -w     | Warning. Provides warnings for small stuff in your programs if there are any.         |
| -d     | Debug. Lets you interactively step through your program to help you debug it (maybe). |

---
## How to use

Simply clone this repository, `cd` into it, run `make` and voilà! You have the binary in the `build` directory.

You can copy it to `/usr/bin` to be able to use it from anywhere, or just use it directly. In the examples below, I'll assume you are in the directory of the repository and using it directly from the `build` directory.
```bash
$ ./build/tack my_program.tk
$ ./build/tack -d my_program.tk
$ ./build/tack -v my_program.tk
$ ./build/tack -w yo.tk
$ ./build/tack -vw hello.tk
```

---
## Sample programs

Here are some simple programs written in *TACK*.

#### Hello:

```tack
# All execution starts from `main` label
# Programs lacking `main` will cause a parse-time error.
main:
  push 0   # Sentinal for jmpz
  push 10  # ASCII value of newline
  push 'o'
  push 'l'
  push 'l'
  push 'e'
  push 'H'

# Stack [0, 'o', 'l', 'l', 'e', 'H']
# breaks when printc has consumed
# all characters and only `0` at
# the start remains.
print:
  printc
  jmpz done
  jmp print

# Stack []
done:
  exit
```

#### Table of n:

```tack
main:
  # Prompt
  push ' '
  push '>'
  printc
  printc

  readn  # Read a number for table
  push 0 # Stack [n, current = 0]

# Stack invariant: [n, current]
# breaks when `current > n * 10`
table:
  # Increment current and print it
  over    # [n, current, n]
  add     # [n, current += n]
  dup     # [n, current, current]
  printn  # [n, current]

  # Print newline
  push 10   # [n, current, 10]
  printc    # [n, current]

  # Check the condition
  over        # [n, current, n]
  push 10     # [n, current, n, 10]
  mul         # [n, current, n * 10]
  over        # [n, current, 10n, current]
  gt          # [n, current, 10n > current]
  jmpz done   # [n, current] if jumps else [n, current, boolean]
  pop         # [n, current] boolean got popped
  jmp table

done:
  # Stack cleanup [n, current]
  pop # [n]
  pop # []
  exit
```

