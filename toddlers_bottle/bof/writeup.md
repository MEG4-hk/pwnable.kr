# bof

***Nana told me that buffer overflow is one of the most common software vulnerability.</br>
Is that true?***

## Recon

With this challenge, we get a few things to start with:

1. A hint -> suggests this challenge is about buffer overflow (bof in short).
1. 2 links to download this challenge's files:
	- [bof binary](http://pwnable.kr/bin/bof)
	- [bof.c source](http://pwnable.kr/bin/bof.c)
1. Connection to the server where the binary is hosted -> `nc pwnable.kr 9000`

This challenge we did not get an ssh connection, but an address and a port to connect</br>
via the **netcat** utility.

Since buffer overflow vulnerability is the main concept of this challenge, let's start</br>
with understanding what and how it works.

#### Understanding concepts

* *buffer* - An array of bytes with a fixed size (usually a simple string).
* *buffer overflow* - Since a buffer has a fixed sized, if we try and insert more data</br>
than it can contain, we **overflow** it, and might write parts of the data we entered</br>
on different sections of the memory we shouldn't have access to write to.
* *stack* - A section of a program's memory. For this challenge, we need to know that</br>
it stores some important things:

	* Function's local variables.
	* Return address (not necessary to know for this challenge).
	* Function's parameters.

So, for example, a stack may look something like this:

	0xFC: param1
	0xF8: param2
	0xF4: ret_addr
	0xF0: local1
	0xEC: local2

### Source files an binaries

With this challenge come 2 links to download these files:

	bof, bof.c

We have a C source and a binary.

#### Understanding the source

There isn't a lot of code in the source file.</br>
The main function only calls another function `func` with a hard-coded value as an</br>
argument and exits the program.

	func(0xdeadbeef);

The `func` function doesn't return a value and recieves a signle integer value.

	void func(int key)

The interesting part is inside `func`.

A local variable `overflowme` is declared, a string containing 32 bytes.

	char overflowme[32];

Right after the declaration, the program recieves a string input from the user into the</br>`overflowme` buffer.

	gets(overflowme);

After reading the input into the buffer, the program compares the function's parameter</br>
`key` with the hard-coded value `0xCAFEBABE`. If matches, the program starts an</br>
interactive bash session.

	if(key == 0xcafebabe){
		system("/bin/sh");
	}

### Identifying a target

Let's return for a moment to `func`'s source code. Notice that the function used to</br>
get a string input from the user is `gets`. Quick research about the function teaches</br>
us it is very unsafe, since there is no size limitation on how much to copy into the</br>
buffer from the user input.</br>
With that said, in theory, we can enter as many bytes as we want and it will (try to)</br>
copy all of them into the given buffer, `overflowme`.

But wait, what good does it do for us to be able to write to the memory after the end</br>
of the buffer? what even is this memory and what's in it?

This memory is the stack, which is explained (briefly) in the #Understanding&#32;conpects</br>
section. The important thing is that parameters are stored in the stack.

The stack of the `func` function should look simmilar to this:

	key
	ret_addr
	[32 bytes]
	overflowme

With this in mind, we understand that if we write enough data into `overflowme`, we</br>
can potentially write over the `key` parameter and change its value.

Therefore, we set our target to overflow the `overflowme` buffer and change the value</br>
of `key` to `0xCAFEBABE`

### Identifying interactions

We interact with the program through normal user input.</br>
Inspection of the source code shows us we can give the program a string - expected to</br>
be 32 bytes long including a nullbyte. In reality, this string can be of any size we</br>
want it to be.

## Solving the challenge

For this challenge we need a debugger tool to inspect the execution of the program</br>
and understand how we can exploit the possible buffer overflow bug we found.</br>
To make it easier, I compiled the source file myself with the `-g` flag.

To do this, I will be using `gdb`, a powerful debugger for GNU/Linux.</br>
Let's open gdb:

	$ gdb ./src/bof

Inside gdb, we can put breakpoints in points of interest for us.</br>
The first point is line 7, where we give the program input and overflow the buffer.</br>
Another point is line 8, where we compare the `key` variable to `0xCAFEBABE`.

	(gdb) b 7
	Breakpoint 1 at 0x7b5: file src/bof.c, line 7.
	(gdb) b 8
	Breakpoint 2 at 0x7c6: file src/bof.c, line 8.

Now, we can run the program and stop at the first breakpoint.

	(gdb) r
	Starting program: ~/.../pwnable.kr/toddlers_bottle/bof/src/bof 
	Breakpoint 1, func (key=-559038737) at src/bof.c:7
	7		gets(overflowme);	// smash me!

Let's view the value of the `key` variable and the `overflowme` string now, before</br>
we overflow it.

	(gdb) x/x &key
	0x7fffffffdd2c:	0xdeadbeef
	(gdb) p overflowme
	$1 = "... [junk bytes] ..."

Continue the execution of the program, then enter a long string.

	(gdb) c
	Continuing.
	overflow me : AA11AA22AA33AA44AA55AA66AA77AA88AA99BB11BB22BB33BB44BB55BB66BB77BB88BB99

By examining the value of `key` again, we can see that its value has changed.

	(gdb) x/x &key
	0x7fffffffdd2c:	0x35354242

The value of `key` has changed! but to what?

`0x35354242` is equal to the string `BB55`. Therefore, if we change only the `BB55`</br>
part in the string we've entered, to the value of `0xCAFEBABE`, we will solve the </br>
challenge! Counting the number of bytes before the `BB55` part gives us an offset of</br>
52 bytes from the end of the stack.

So, we send 52 bytes of junk and then 4 bytes with the value `0xCAFEBABE`. Then, we</br>
get a shell on the server, and able to read the flag.