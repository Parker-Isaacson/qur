# Qur Language

**Qur** (pronounced *“cure”*) is a small experimental language inspired by C++ and designed to explore lexical analysis, parsing, and AST generation. Qur source files end with `.qur`.

This implementation includes:

* A **lexer** that tokenizes Qur syntax into meaningful tokens
* An **AST builder** that constructs an abstract syntax tree
* Basic support for **functions, loops, conditionals, literals, assignments, and operators**

---

## Example Program

```qur
fn int main() {
    string msg = "Hello, World!";
    print(msg);

    int a = 5;
    int b = 10;

    if (a < b) {
        print("a is less than b");
    } else {
        print("a is not less than b");
    };

    for (int i = 0; i < 5; i++) {
        print("Loop iteration");
    };

    return 0;
};
```

Running the lexer and parser on this file will print the token stream and build a full AST representation of the program structure.

---

## Language Overview

### Functions

All Qur programs begin execution in `fn int main()`.
Functions are declared using the `fn` keyword, followed by a **return type**, a **name**, and **parenthesized parameters**.

```qur
fn void greet(string name) {
    print("Hello, ${name}");
};
```

A function must always have a body enclosed in `{}` and end with a semicolon `;` after the body.

---

### Variables and Types

| Qur Keyword | Meaning / Equivalent Type |
| ----------- | ------------------------- |
| `int`       | integer                   |
| `double`    | floating point            |
| `boolean`   | true/false                |
| `char`      | single character          |
| `string`    | text string               |

Example:

```qur
int count = 5;
double ratio = 3.14;
boolean flag = true;
char letter = 'A';
string text = "Hello";
```

---

### Operators

The following operators are currently supported by the lexer and AST:

| Category            | Operators                         | Example        |              |
| ------------------- | --------------------------------- | -------------- | ------------ |
| Assignment          | `=`, `+=`, `-=`, `*=`, `/=`, `%=` | `x += 1;`      |              |
| Arithmetic          | `+`, `-`, `*`, `/`, `%`           | `y = a + b;`   |              |
| Comparison          | `<`, `>`, `<=`, `>=`, `==`, `!=`  | `if (x >= 10)` |              |
| Logical             | `!`, `&`, `                       | `, `~`         | `if (!flag)` |
| Increment/Decrement | `++`, `--`                        | `x++; ++x;`    |              |

---

### Conditionals

Qur supports `if`, `elif`, and `else`.
Each block must be enclosed in `{}` and the entire statement must end with `;`.

```qur
if (x == 0) {
    print("Zero");
} elif (x == 1) {
    print("One");
} else {
    print("Other");
};
```

---

### Loops

#### `for` loops

Qur supports both classic for-loops and iterator style loops.

```qur
for (int i = 0; i < 3; i++) {
    print("Count: ${i}");
};
```

#### `while` loops

```qur
int i = 0;
while (i < 3) {
    i++;
};
```

---

### Function Calls and Return

Function calls can take arguments, and `return` statements may include expressions.

```qur
fn int add(int x, int y) {
    return x + y;
};

fn void main() {
    int result = add(2, 3);
    print(result);
};
```

---

### Strings and Escape Sequences

String literals support escape sequences and interpolation syntax (`${}`).

```qur
string msg = "Line 1\nLine 2\tTabbed";
print(msg);

string name = "Qur";
print("Hello, ${name}!");
```

---

### Example: Combined Syntax Test

Below is a sample `gentest.qur` that exercises most supported features:

```qur
fn void testIncrements() {
    int x = 0;
    print("Testing ++ and --");
    x++;
    --x;
    print("Done");
};

fn void testAssignments() {
    int a = 5;
    a += 2;
    a -= 1;
    a *= 3;
    a /= 2;
    print("Assignments done");
};

fn int main() {
    testIncrements();
    testAssignments();
    return 0;
};
```

---

## Building & Running

### Prerequisites

* C++17 or later

### Build

```bash
make
```

### Run

```bash
./compiler -c testcases/gentest.qur
```

Output includes:

* Lexical tokens
* AST representation
* (Optional) debug parse info

---

## Project Structure

| File                  | Description                                       |
| --------------------- | ------------------------------------------------- |
| `lexer.h / lexer.cpp` | Lexical analyzer: tokenizes Qur source            |
| `ast.h / ast.cpp`     | Abstract Syntax Tree builder and node definitions |
| `qur.cpp`             | Program entry and orchestrator                    |
| `gentest.qur`         | Sample source code used for demonstration         |
| `README.md`           | Language syntax guide and usage instructions      |
