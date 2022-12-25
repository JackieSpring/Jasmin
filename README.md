# Jasmin
Jasmin Assembly Interpreter
This tool allows you to write assembly instructions and execute them in real time directly from the command line; It is able to support different architectures and syntax, it provides utility commands for debugging the code and manipulation of the execution environment.
At the moment it only supports a small number of instructions for the x86 64 or 32 -bit architecture architecture with Intel or Gas syntax.

### Installation
At the moment, jasmin can run only on \*nix systems, Windows support is not implemented yet.
To compile Jasmin are needed __unzip__, __make__ and __cmake__ programs, so make sure you have installed them before continuing with the installation
1. Download source from github
2. Navigate in the directory
3. Run ./init.sh

### Usage
To start Jasmin, simply use this command in the Jasmin directory
```sh
./jasmin
```
As a default setting uses 64 -bit x86 architecture with intel syntax, different syntax and/or architectures can be set to start -up:
```sh
./jasmin -m 	# use 32-bit architecture
```
```sh
./jasmin -g 	# use x86 GAS syntax
```
Once started, you will be prompted to a command line where you can enter your assembly instructions and control commands:
```
(running) >>> 
```
At this point you can execute assembly instructions simply by typing them and pressing Enter:
```
(running) >>> mov rax, 0xdeadbeef
(running) >>> <print regs
rflags   0000000000000202
rax      00000000deadbeef
rbp      0000000000000000
rbx      0000000000000000
rcx      0000000000000000
rdi      0000000000000000
rdx      0000000000000000
rip      000000000804800a
rsi      0000000000000000
rsp      00007ffffffe9000
(running) >>> hlt
(inactive) >>> < exit
# jasmin exited
```
The syntax of the commands is "< *command*",  the __print__ command (used to print useful information such as the status of registers, memory or interpreter, but not only), and the __exit__ command, useful to terminate jasmin, were used in the example.
The __help__ command will give you the list of commands and information on their use.