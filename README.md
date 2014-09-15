E--_compiler
============

Compiler for language E--. Developed as a course project CSE504 Spring 2014.

INSTALLATION
-----------------------------------
1. run make
2. make a <filename>.e file and write your code into it
3. run ./parser <filename>.e (it will generate <filename>.i (i.e low level code) and <filename>.q (i.e intermidiate code)
4. rum ./parser <filename>.e prttype (to see typecheck messages and output)
5. run ./easm <filename>.i (Make sure there are no errors, only warnings)
6. run ./erun <filename>.i -dr -df -m 10000000 -dm 9990 10000 > <filename>.out 2> <filename>_err.out < test1.in
    here <filename>.out is output file, <filename>_err.out is error file and test1.in is input file.

Test Cases Included
-----------------------------------
1. type check test cases in "typecheck/test/" folder
2. Code geneartion test cases in "finalTests" folder
    a. anycheck.e - Checks if any event works
    b. funccall.e - Checks if function call works
    c. test_if_else.e  - Checks if if/else works 
    d. test_recur.e - Checks if recurssion works
    e. test_shift.e - Checks SHL, SHR operator
    f. test_while.e - Checks multiple nested while/break 
    g. whilecheck.e - Checks simple while block
    h. opt.e - Checks normal operators and Common subexpression elimination optimization
    i. expr.e - Checks for large expressions
    
    All test cases contain <filename>.out files with expected output.

3. Input file
------------------------------------
    a. test1.in - contains two event inputs.
	i.  a(75, 11.0)
	ii. a(80, 12.0)


