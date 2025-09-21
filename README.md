# Qur Language

Qur, pronounced "cure", based upon C++. Qur files are ended with `.qur`.

## Basics

Qur has everything delimited by a `;`, lines, loops, functions. The standard entry point is `fn int: main() {};`, arguments can be provided with the following. The `main` function can only exist once.

```qur
import list;
fn int: main(int: argc, string[]) { return 0; };
```

### Primitive Variable Types

| qur | C++ | Notes |
| :--- | :--- | :--- |
| int | int | Not supported in an `if`, check value literally. |
| double | double | Not supported in an `if`, check value literally. |
| boolean | bool |
| char | char |
| string | std::string |
| list | std::vector |
| tuple | std::tuple |
| dict | std::map |
| type | template\<typename T\> | Allows for user defined types. |

```qur
fn int: main() {
    int: myInt = 0;
    double: myDouble = 3.14;
    boolean: myBool = false;
    char: myChar = 'c'
    string: myString = "Hello, World!";

    type: myType = int;
    return 0;
};
```

String interpretation is possible, use `${}`, if the inside is a numeric value the rounding amount can be provided with `:n` where `n` is the amount of digits to round. The inside cannot be used for computation.

```qur
fn int: main() {
    double: val = 3.141595;
    print("${val:2}"); // Prints "3.14"
    return 0;
};
```

### Default Operators

| qur | C++ | Type | Usage |
| :--- | :--- | :--- | :--- |
| = | = | Assignment | set |
| -> | : | Assignment | for each |
| + | + | Arithmetic | addition |
| - | - | Arithmetic | subtraction |
| * | * | Arithmetic | multiplication |
| / | / | Arithmetic | division |
| % | % | Arithmetic | modulo |
| < | < | Relational | less than |
| > | > | Relational | greater than |
| <= | <= | Relational | less than or equal to |
| >= | >= | Relational | greater than or equal to |
| == | == | Relational | equal to |
| != | != | Relational | not equal to |
| ! | ! | Logical | not |
| & | && | Logical / Bitwise | and |
| \| | \|\| | Logical / Bitwise | or |
| ~ | ~ | Bitwise | invert |

### if, elif, else

Checking conditions is essential, an `if` `else if` chain only ends when the final condition has a `;`, the `elif` or `else` does not need to be on the same line as the ending `}`.

```qur
fn int: main() {
    int: val = 1;
    if ( val == 0 ) {
        // Wont get here
    } elif ( ! ( val == 1 ) ) {
        // Wont get here
    } else {
        return 1;
    };
    return 0;
};
```

### For and While Loops

Loops are declared in the similar method as you would in C. Must be closed with a `;`. Variables declared inside the `for` are scoped to only be inside the next bracket set.

```qur
fn int: main() {
    // Conditional for loop
    // Variable; Condition; Step;
    for ( int: i = 0; i < 10; i++; ) {
        // Do something
    };
    // For each loop
    list: myIntegers(int, 5) = [1, 2, 3, 4, 5]
    for ( int: i -> myIntegers) {
        // Do do something
    };
    // Condition;
    while ( true; ) {
        // Do Something
    };
    return 0;
};
```

### Functions, Arguments, and Operators

Functions are declared in the following methods. Functions that have a type must return a value. When a type is not specified the function cannot return any value. Functions must have a body, and cannot be declared in one place and have a body in another. Functions can have the same name and different arguments, so C++ overloading.

```qur
// Integer Function
fn int: func() { return 0; };

// Void Function
fn: func() {};
```

Operators are required to have at least 1 argument and a return type. With only 1 the value is taken from the right of it, think `! true`. With 2 arguments they are from the left and right, think `0 != 1`. Customer operator functions can be defined with the `op` keyword, the custom operator must be surrounded by \` the insides can be any character set.

```qur
op int: `add`(int: arg1, int: arg2) { return arg1 + arg2; };
op boolean: `not`(boolean: arg1) { return !arg1; };

fn main() {
    int: val1 = 0;
    int: val2 = 2;
    int: res = val1 `add` val2;
    boolean: truthy = `not` false;
    return res; // 2
};
```

### Default Libraries

`io` is a library that provides input for input and output to the command line, though is not essential to import for `print` or `read`, though it will allow for more advanced file io, and control over the cli.

```qur
import io;
fn int: main() {
    /* Prints to the command line.
     * Does print a '\n' at the end.
     */
    print("Hello World!");
    // If you don't want the '\n', call print() with false as a second argument
    print("Hello World!", false);
    /* Read always returns a string
     * argument is the amount of characters returned
     */
    string: userInput = read(5);
};
```

`list`, `tuple`, `dict` are included by default, though can be imported for brevity. `list` and `dict` are mutable, `tuple` has mutable values but immutable size. `list` and `tuple` are zero indexed, and use `[]` and `()` respectively. A size can be provided to `list` as shown below, but is not needed. A size must be provided to `tuple` as shown below. `list` and `dict` are dynamically sized. 

```qur
fn int: main() {
    // List size does not need to be provided.
    list: myList(int, 3) = [1, 2, 3]; // myList[1] == 2
    tuple: myTuple(int, 3) = (1, 2, 3); // myTuple(2) == 3
    dict: myDict(int, string) = {0: "zero", 1: "one"}; // myDict{1} == "one"
    return 0;
};
```

### Imports

Importing libraries are essential, done simply with the `import` keyword during the files header. This will import all functions, classes, and interfaces. If the library is not found in the default library location, what is input will be treaded as a relative path from where the source file is. Imports do cascade through files, for instance if `myLib` imports `list` then the following will also have `list`.

```qur
import ./myLib; // Assume this has func1()

fn int: main() {
    int: res = func1();
    return res;
};
```


# TODO: ALL BELOW

Following section is currently theoretical.

### Binary options 

`$ qur [-c (--compile)] file.qur` outputs `file` as an executable

`$ qur -o (--out) file` sets the output file

`$ qur -d (--download) lib` downloads a library, and stores in the default library location

`$ qur -h (-?, --help)` prints help

## Possible additions

- class
- interfaces
- structs
- enumerations
- unions
- switch
- error handling
- Concurrency primitives ( threads, async, atomic, mutex / locks)
- system functions

### Classes, Objects, Interfaces

OOP principles are essential. Classes are supported, by default internal variables and methods are private. Only private and public exist, if you inherit from a class you can only see public variables and methods, inheritance from an interface will let you see both public and private. Interface methods must have a body, even if it is overwritten by the child class. Constructors must be public and have the same name as the class, they are defined like a regular function.

```qur
class: myClass {
    private {
        // Variables and Members
    };
    public {
        // Variables and Members
        fn int: myClass(int; arg1) { 
            myVar = arg1;
            return 0; 
        };
        int: myVar;
    };
};

interface: myInterface {
    private {
        // Variables and Members
    };
    public {
        // Variables and Members
    };
};

class: myInheritedClass(myInterface) {
    // This will have all variables and methods from myInterface.
};

fn int: main() {
    myClass: myObject();
    print("${myClass.arg1}")
    return 0;
};
```