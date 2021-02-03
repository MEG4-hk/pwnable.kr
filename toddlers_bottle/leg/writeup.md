# leg

***Daddy told me I should study arm.</br>
But I prefer to study my leg!***

## Recon

This challenge gives as a few starting points:

1. An obfuscated hint -> This challenge is about ARM assembly.
2. links to download:

	- [C source](http://pwnable.kr/bin/leg.c)
	- [Assembly source](http://pwnable.kr/bin/leg.asm)
3. Connection to the server via ssh -> `ssh leg@pwnable.kr -p2222 (pw:guest)`

#### Understanding concepts

This challenge is about ARM assembly. But what is ARM assembly?

* *ARM* - In short, ARM is a different platform than Intel x86 processors. It is a</br>
RISC processor, unlike the CISC Intel processors.
* *RISC* - **R**educed **I**instruction **S**et **C**omputing is computing in an</br>
environment that has much less instructions than usual (ARM features about 100).
* *ARM assembly* - This is a different assembly language than Intel's x86, with</br>
much less instructions but more registers.

### Source files an binaries

Connecting to the server via ssh shows us we are not connected to any user like</br>
other challenges, we are currently at the `busy` user.

	$ whoami
	busy

Let's see where are we.

	$ pwd
	/
	$ ls
	bin/, dev/, flag/, linuxrc, root/, sys/, boot/, etc/, leg, proc/, sbin/, usr/

We found a binary, `leg`. We also have 2 download links to the source code files.

	leg.c, leg.asm

We have a C/asm source and a binary.

#### Understanding the source

Let's take a first look at the C source.

It has 3 functions named `key1`, `key2` and `key3`, all returns a number.

1. `key1` function:

	The function has only one line of assembly code in it. What it does is set the</br>
	value of the `r3` general purpose register with the value of the `pc` register.

		asm("mov r3, pc\n");

	The `pc` register in ARM is similar to the `ip` register in Intel x86 instruction</br>
	set. It holds the address of the next instruction to be executed. Now, we can</br>
	understand that this function's return value is the value of the `pc` register at</br>
	the moment of running this function.

2. `key2` function:

	This function is the largest of the three. Basically, it does the same thing as</br>
	`key1`, but adds 0x4 to the value of `pc`.

		asm(
		"push	{r6}\n"
		"add	r6, pc, $1\n"
		"bx	r6\n"
		".code   16\n"
		"mov	r3, pc\n"
		"add	r3, $0x4\n"
		"push	{r3}\n"
		"pop	{pc}\n"
		".code	32\n"
		"pop	{r6}\n"
		);

	This function's return value is the value of the `pc` register (with the addition</br>
	of `0x4`) at the moment when the `mov r3, pc` line is executed.

3. `key3` function:

	This function is similar to both `key1` and `key2` functions, but returns the</br>
	value of `lr`, the link register. This register holds the return address, which</br>
	is equal to the address of the instruction to execute after the function call.

		asm("mov r3, lr\n");

	This function's return value is its return address.

In the main function, the program asks the user to input a number into a local</br>
variable named `key`. Then, it comares `key` with the sum of the three values</br>
returned from the three functions `key1`, `key2` and `key3`. If matches, we get</br>
the flag.

	scanf("%d", &key);
	if( (key1()+key2()+key3()) == key ){
		printf("Congratz!\n");
		int fd = open("flag", O_RDONLY);
		char buf[100];
		int r = read(fd, buf, 100);
		write(0, buf, r);
	}

### Identifying a target

Since we get the flag by entering a number that is equal to the sum of the return</br>
values from the three functions, we first need to find these return value, then sum</br>
them, and enter the result as the key to the program. Not so bad!

### Identifying interactions

We have a single way of interacting with the program via user input. This is where we</br>
need to enter the matching key to get the flag.

## Solving the challenge

For each of the three functions, let's find the corresponding return value.

1. `key1` function:
	
	Looking at the [`leg.asm`](src/leg.asm) file, we can see the disassembly of the</br>
	program. We found locate the line of the `key1` function:

		0x00008cdc <+8>:	mov	r3, pc
		0x00008ce0 <+12>:	mov	r0, r3
		0x00008ce4 <+16>:	sub	sp, r11, #0

	Our line is at `0x00008cdc`. The value of `pc` at this instruction can have two</br>
	options. The ARM assembly, there are two modes: ASM mode and Thumb mode. In ASM</br>
	mode, the value of `pc` is the address of the next instruction to execute. In</br>
	Thumb mode, it is the address of the second next instruction, **skipping the</br>
	next function**. We cannot know what mode the machine is running at, so we will</br>
	try both.

	*After trying both, we discovered that the machine is set to Thumb mode, so for</br>
	the rest of the writeup we will address the machine like we know it is set to</br>
	Thumb mode.*

	That means, the return value of this function will be `0x00008ce4`.

1. `key2` function:

	Same as in `key1` function, we will look at the disassembly to get the return</br>
	value of this function.

		0x00008d04 <+20>:	mov	r3, pc
		0x00008d06 <+22>:	adds	r3, #4
		0x00008d08 <+24>:	push	{r3}

	This function's return value will be `0x00008d08 + 0x4`.

1. `key3` function:

	This time we do not need to actually look inside the function, but in the `main`</br>
	function. Our return value is the return address, which means the address of the</br>
	instruction next to the function call.

		0x00008d7c <+64>:	bl	0x8d20 <key3>
		0x00008d80 <+68>:	mov	r3, r0

	This function's return value will be `0x00008d80`.

To calculate the key we need to enter, we will sum up all of the return value:

	key = 0x00008ce4 + (0x00008d08 + 0x4) + 0x00008d80
	key = 108400

Entering this key, we get the flag.

## Notes

This challenge's machine is wierd. The exploit script works, but sometimes the output</br>
from the machine get's broken and doesn't the flag, even though it's printed on the</br>
machine.

If you use the exploit script just run it a few times and it should be fine. If not,</br>
just connect to the server yourselves via ssh, run the program and enter the key</br>
It is manually, but it will work.