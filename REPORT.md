ECS 150 -  Fall 2019

Emil Tokaev
Vadzim Matsiushonak

Project 1
10/15/2019

# Brief description

The prompt required us to create a shell in C programming language. The main 
features of the project included: taking input from stdin, parsing it,
handling builtin commands (exit, pwd, cd), input and output redirection, 
piping and background commands. 

# Major features

Most shell's operations depend heavily on taking command line input, parsing
and saving it. For this reason vast majority of shell processes work with the
command line input. Each command forks into its own process of execution so 
as not to alter the main shell process. Our implementation could be improved
by splitting our shell.c file into several separate files containing data
structures, main function and other functions. 

# Parsing user input

User input is parsed and stored into a linked list each node of which
represents a single command and its attributes. When piped commands 
encountered, the input is broken down into a number of c-strings
(single commands) each of them is then parsed and stored into a linked list. 
The linken list's nodes' members are:
	* An array containing a command's name and command's arguments
	* A pointer to the next node in linked list
	* Process id number
	* Process exit status

Building a command node in linked list takes three steps. First, the original
user input is separated by the pipe character into a number of strings. 
Each string is then checked for redirection. If redirection is found we remove
redirection sign and save redirection file name to make execution easier. 
After that the remaining arguments are divided into single strings and stored 
in command node's array of arguments. However, due to time constraints, 
we were unable to solve several error checks and corner cases, like no 
input file, no output file, mislocated output redirection. 

We used to the following online sources for our project:
	* <https://www.geeksforgeeks.org/linked-list-set-2-inserting-a-node/>
	* <https://codeforwin.org/2016/04/c-program-to-trim-leading-white-
spaces-in-string.html>
	* <https://gist.github.com/mplewis/5279108#file-threepipedemo-c>

# Processing commands

After creating an array of command node we process these commands. We followed
the phases described in the project prompt. We first executed single commands,
then checked for redirections in single commands. After that we worked on
piping in the later phases. We encountered problems in keeping our code 
simple and managing errors as we process single commands, built in commands, 
redirections, and pipes in separate functions. At the later stages the major 
flow of execution was about extracting pipes and executing pipe commands. 

# Testing

We ran our code after every minor change, used our own test cases and sample
code from the prompt as well as given test script.

# Conclusion

We faced some difficulties working on the project as both of us lacked 
substantial coding experience during last 1-2 years and coding experience in C
in particular. It also took us time to refresh on using git and on creating 
Makefile for the project. 
