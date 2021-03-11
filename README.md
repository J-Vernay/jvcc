# jvcc - A C compiler project

**jvcc** is a hobby project to make a C compiler from standard C.
Its aim is to be self-compiled.

The priorities of the project are ranked as below:
1. Code must be readable and the logic should be simple.
2. The compiler itself is written in portable C (but not necessarily for tests and examples).
3. Compliance with latest ISO C.

## How to use it

To compile the project:
```
mkdir build          # create a build/ directory
cd build             # going to this directory
cmake ..             # configuring the project
cmake --build .      # compiling the project
./jvcc               # run jvcc 
```

To launch the tests (Python 3 required):
```
cd tests # go to direcory integration-tests/
python3 -m unittest discover
```

Examples of usage:
```
./jvcc tokenize output-tokens.txt input.c --skip-spaces
```

## Current features

## Next tasks to be done

- [ ] Lexer (separating source code into tokens)
- [ ] Preprocessor
- [ ] Parsing literals
- [ ] Parsing expressions
- [ ] Generate Abstract Syntax Tree