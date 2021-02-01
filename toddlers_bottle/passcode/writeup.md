# passcode

***Mommy told me to make a passcode based login system.</br>
My initial C code was compiled without any error!</br>
Well, there was some compiler warning, but who cares about that?</br>***

## Recon

In this challenge, we recieve 2 things, like usual:

1. A hint -> suggests something about a compiler warning, we should pay attention to it.
1. Connection to the challenge's machine -> `ssh passcode@pwnable.kr -p2222 (pw:guest)`

To complete this challenge there are a few concepts that we first need to understand.

#### Understanding concepts

* *ASLR* - **A**ddress **S**pace **L**ayout **R**andomization is the concept of </br>
programs running in random addresses each time executed, for security reasons.
* *GOT* - **G**lobal **O**ffset **T**able is a program's section in memory that holds</br>
addresses of external procedures or functions (usually from the standard C library,</br>
such as `puts` or `fflush`).</br>
It enables programs to run independent of the memory address they are loaded at</br>
on runtime.
* *PLT* - **P**rocedure **L**inkage **T**able is used to call external procedures</br>
or functions whose address isn't known in the time of linking, and is left to be</br>
resolved by the dynamic linker at run time.

### Source files and binaries

We'll connect to the server via ssh.

	$ ls
	passcode, passcode.c, flag

We have a C source and a binary.

#### Understanding the source

The main function is very straight-forward, calling 2 functions, not much else.

- `welcome`
- `login`

Inside the `welcome` function, a local 100 bytes long string variable called `name`</br>
is declared, then filled with input from the user.

	char name[100];
	printf("enter you name : ");
	scanf("%100s", name);

Inside the `login` function, 2 local variables of type int are declared, `passcode1`</br>
and `passcode2`.

Afterwards, the program asks the user to enter a number into `passcode1`, but there is</br>
no `&` to pass the address of `passcode1` to the `scanf` function, and the junk value</br>
that was already in `passcode1` is passed as an `int*`. After the broken `scanf`, the</br>
`fflush` function is called to flush `stdin`.

	printf("enter passcode1 : ");
	scanf("%d", passcode1);
	fflush(stdin);

Similar to `passcode1`, `passcode2` is also being called with a broken `scanf` call.

	printf("enter passcode2 : ");
	scanf("%d", passcode2);

Now, the program checks if the values of `passcode1` and `passcode2` are equal to some</br>
hard-coded values. If matches, the program prints the flag.

	if(passcode1==338150 && passcode2==13371337){
		printf("Login OK!\n");
		system("/bin/cat flag");
	}

### Identifying a target

After reading the source code, we understand that our target is to set the value of</br>
`passcode1` to `338150`, and the value of `passcode2` to `13371337`.

**But**, there's a problem, since we can't enter any numbers into `passcode1` or</br>
`passcode2` using the broken `scanf` calls. We need to find a different target.

Examining the source code closer, we can see that the junk value of `passcode1` can</br>
probably be controled using the `name` buffer from the `welcome` function.</br>
With this potential vulnerabillity in mind, we can think of a way to print the flag</br>
without actually having the two passcode variables set to what they need to be (`338150`</br>and `13371337`).

Basically, we have a situation that we can write arbitrary 4 bytes we control, to any</br>
4 bytes address we want. That means, the program is exposed to GOT hijacking attack,</br>
where we as an attacker change the value of one of the entries in the GOT to another</br>
address which contains code that we want to execute. In this case, line 19:

	system("/bin/cat flag");

When searching for a good candidate to overwrite in the source code, we spot again</br>
that `fflush` is being called right after the first broken `scanf` call.

In conclusion, our new target is use the broken `scanf` call to overwrite the address</br>
of `fflush` to the address of line 19 and print the flag.

### Identifying interactions

We have 3 interactions with the program:

1. In `welcome` function, we can write 100 bytes into a local buffer in the function.
1. In `login` function, we can enter a number into wherever `passcode1`'s junk value</br>
points to.
1. In `login` function, we can enter a number into wherever `passcode2`'s junk value</br>
points to.

## Solving the challenge

Ok! Now we have an interesting target. What are we doing now?

First thing first, we need to find where exactly inside the `name` buffer lies `passcode1`'s</br> junk value.

Next thing, we need to find 2 addresses:

1. The address of fflush in the GOT that we want to overwrite.
1. The address of line 19, that we want to write to the first address we find.

All 3 of these things can be done using a debugger, like `gdb`.

Let's open the program in gdb:

	$ gdb src/passcode

Disassembling `login` and `welcome` functions shows us that the `passcode1` variable</br>
is at offset 96 of the `name` buffer, and `passcode2` is at offset 100. This means</br>
that we were right, we can control the value of `passcode1` but not `passcode2`.

Now, finding the two addresses we are looking for.

To find the address of `fflush` in the GOT, let's print it first

	(gdb) p fflush
	$1 = {<text variable, no debug info>} 0x8048430 <fflush@plt>

Using this address, we can find the address of `fflush` in the GOT. Let's see what's</br>
inside this address:

	(gdb) x/i 0x8048430
	0x8048430 <fflush@plt>:	jmp    *0x804a004

We got another address! lets look inside it too.

	(gdb) x/x 0x804a004
	0x804a004 <fflush@got.plt>:	0x08048436

Great! We found the address we want to overwrite: `0x804a004`. Overwriting the value</br>
inside it to any other value will make the program's execution flow to jump to the</br>
value we overwrite it with, for example, the address of line 19.

Speaking of which, lets find this address.

	(gdb) disas login
	...
	0x080485e3 <+127>:	movl   $0x80487af,(%esp)
	0x080485ea <+134>:	call   0x8048460 <system@plt>
	...

The address we want is `0x080485e3`, since it is the address where we push the</br>
string parameter "/bin/cat flag", and right afterwards call `system`.

Wrapping it up, we are going to construct a payload, build from 96 junk bytes, then </br>
the value `0x804a004`. Then, we will enter as input the address `0x080485e3`. This</br>
will overwrite the address of `fflush` in the GOT and replace it with the address of</br>
line 19, calling the `system` function with the string parameter `"/bin/vat flag"`,</bt>
which will print the flag. Challenge solved!