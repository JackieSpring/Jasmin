# Jasmin
Jasmin Assembly Interpreter
This tool allows you to write assembly instructions and execute them in real time directly from the command line; It is able to support different architectures and syntax, it provides utility commands for debugging the code and manipulation of the execution environment.
At the moment it only supports a small number of instructions for the x86 64 or 32 -bit architecture architecture with Intel or Gas syntax.
- [Installation](#Installation)
- [Usage](#Usage)
	 - [Basic usage](#Basic-usage)
	 - [Labels and symbols](#Labels-and-symbols)
	 - [Execution state and breakpoints](#Execution-state-and-breakpoints)
     - [Functions](#functions)
	 - [Limitations](#Limitations)

### Installation
At the moment, jasmin can run only on \*nix systems, Windows support is not implemented yet.
To compile Jasmin are needed __unzip__, __make__ and __cmake__ programs, so make sure you have installed them before continuing with the installation
1. Download source from github
2. Navigate in the directory
3. Run ./init.sh

### Usage
#### Basic usage
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
#### Labels and symbols
As in the Assembly programming, Jasmin also allows the use of labels, they are declared to the current instruction pointer and follow the syntax ___[a-zA-Z0-9] :___. An example of use of labels is the creation of loop:
```
(running) >>> xor rax, rax
(running) >>> mov rcx, 3
(running) >>> loop:
(running) >>> add rax, 2
(running) >>> dec rcx
(running) >>> jnz loop
(running) >>> <print regs
rflags   0000000000000256
rax      0000000000000006
rbp      0000000000000000
rbx      0000000000000000
rcx      0000000000000000
rdi      0000000000000000
rdx      0000000000000000
rip      0000000008048013
rsi      0000000000000000
rsp      00007ffffffe9000
(running) >>> < print code
0x8048000       	xor             rax, rax
0x8048003       	mov             rcx, 3
0x804800a       	add             rax, 2
0x804800e       	dec             rcx
0x8048011       	jne             0x804800a
(running) >>> < print syms
__stack           0x7ffffffe9000
__bss             0x804c000
__data            0x804b000
__rodata          0x804a000
__text            0x8048000
loop              0x804800a
(running) >>> 
```
#### Execution state and breakpoints
The program can cross three states:
- __running__
	Executes every stored instruction, if they finish remains waiting; each new instruction inserted will be stored and executed immediately.
- __paused__
	The execution of the program remains on pause, it can be resumed at any time from the current instruction pointer on condition that points towards an executable memory area. The new instructions will be stored but not executed.
- __inactive__
	The program is not being executed, if started it will move on to the running state and its execution will begin from the program's entrypoint. The new instructions will be stored but not executed.

The current state is indicated between the parenthesis of the command prompt and can be controlled through assembly instructions (such as HLT), commands (__run__, __term__, __pause__, __continue__), or events (breakpoints).

In Jasmin it is possible to set breakpoints to pause the execution of the program when the Instruction Pointer reaches a certain memory address, at this point the execution state will go from __running__ to __paused__. To declare a breakpoint you use the __break__ command to which you pass the name of the new breakpoint and the memory address.
```
(running) >>> < print code
0x8048000       	xor             rax, rax
0x8048003       	mov             rcx, 3
0x804800a       	add             rax, 2
0x804800e       	dec             rcx
0x8048011       	jne             0x804800a
(running) >>> < term          
(inactive) >>> < break loopjump 0x8048011
(inactive) >>> < run
-----! Breakpoint hitted !-----
Address: 0x8048011
(paused) >>> 
```
The created breakpoints can be removed with the __delbreak__ command.
#### Functions
Since Jasmin assembles the program sequentially to the user's input, it is not able to refer to symbols not yet created, this can be a problem when you want to create functions; although there is no solution to this problem, it is still possible to carry out a workaround by exploiting the states and entrypoints of the program.
The idea is to declare the functions before the user code without these being executed at the start or during their declaration, and to move the entrypoint of the program under their definitions.
###### 1) block the execution
The status of the program is set to __inactive__ to stop the execution of new instructions and resume it subsequently from the new entrypoint.
```
(running) >>> < term
(inactive) >>>
```
###### 2) define the functions
Define the functions you will go to use in your program.
```
(inactive) >>> sum:
(inactive) >>> mov rdx, [rsp+8]
(inactive) >>> mov rax, [rsp+16]
(inactive) >>> add rax, rdx
(inactive) >>> ret
(inactive) >>>
```
###### 3) reset the entrypoint
After you defined all the functions, reassign the entrypoint at the address immediately following the functions code.
```
(inactive) >>> entry:
(inactive) >>> < print syms
__stack           0x7ffffffe9000
__bss             0x804c000
__data            0x804b000
__rodata          0x804a000
__text            0x8048000
entry             0x804800e
sum               0x8048000
(inactive) >>> < set entrypoint 0x804800e
(inactive) >>>
```
###### 4) start the program
Start the program and resume the execution, if the procedure has been followed correctly, the code of the functions will be ignored to be executed only at the time of the call.
```
(inactive) >>> < run
(running) >>> 
```
###### full example
```
Starting Jasmin v0.1
use <help for help
(running) >>> <term
(inactive) >>> sum:
(inactive) >>> mov rdx, [rsp+8]
(inactive) >>> mov rax, [rsp+16]
(inactive) >>> add rax, rdx
(inactive) >>> ret
(inactive) >>> entry:
(inactive) >>> <print syms
__stack           0x7ffffffe9000
__bss             0x804c000
__data            0x804b000
__rodata          0x804a000
__text            0x8048000
entry             0x804800e
sum               0x8048000
(inactive) >>> <set entrypoint 0x804800e
(inactive) >>> <run
(running) >>> mov rax, 0
(running) >>> mov rcx, 0xca00
(running) >>> mov rdx, 0xfe
(running) >>> push rcx
(running) >>> push rdx
(running) >>> call sum
(running) >>> <print regs
rflags   0000000000000000
rax      000000000000cafe
rbp      0000000000000000
rbx      0000000000000000
rcx      000000000000ca00
rdi      0000000000000000
rdx      00000000000000fe
rip      000000000804802a
rsi      0000000000000000
rsp      00007ffffffe8ff0
(running) >>> <print code
0x8048000       	mov             rdx, qword ptr [rsp + 8]
0x8048005       	mov             rax, qword ptr [rsp + 0x10]
0x804800a       	add             rax, rdx
0x804800d       	ret             
0x804800e       	mov             rax, 0
0x8048015       	mov             rcx, 0xca00
0x804801c       	mov             rdx, 0xfe
0x8048023       	push            rcx
0x8048024       	push            rdx
0x8048025       	call            0x8048000
(running) >>> 
```

#### Limitations
1. With GAS syntax, memory addressing using two or three parameters (base, index, size) is not possible:
```
movl (%eax, %edx, 4), %esi	#Invalid operand
leaq (%rax, %rdx), %rsi		#Invalid operand
add (%rsi), %rax				#OK
```
This problem is not present with Intel syntax
```
mov rax, [rdx+rcx*2]			#OK
```
2. In 64 -bit Intel syntax it is not possible to carry out logical/mathematical operations using immediate size between 5 and 7 bytes. This problem is present in gas syntax.
```
and rcx, 0xffffffffff		#Invalid operand
xor rdx, 0xdeadbeef		#Invalid operand
add rdi, 0xffffffffffffff	#Invalid operand
add rax, 0xffffffffffffffff	#OK
subq $0xffffffffe, %rbx		#OK
```
3. At the moment only the following X86 instructions are supported:
- ADD
- AND
- CALL
- DEC
- HLT
- INC
- Jcc
- JMP (does not affect eflag)
- LEA
- MOV
- MOVABS
- NOP
- NOT
- OR
- POP
- PUSH
- RET
- SUB
- XOR
4. Jasmin is not able to execute system calls since it does not virtualize an operating system, despite using entrypoints and linux systems stackbase.

