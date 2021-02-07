# mistake

***We all make mistakes, let's move on.***

## Recon

Here's the information we get about this challenge.

1. A note (plus credits) about this challenge ->

	> (don't take this too seriously, no fancy hacking skill is required at all)
>
> This task is based on real event
>
> Thanks to dhmonkey

	Interesting. 

1. Server connection -> `ssh mistake@pwnable.kr -p2222 (pw:guest)`
1. A hint -> operator priority.

#### Understanding concepts

* *Operator priotiry* - Arithmetic operator (such as `+`, `-`, `%` etc.) or bitwise</br>
operators (such as `^`, `&` etc.) has priorities, and has an order of execution. [here](https://en.cppreference.com/w/c/language/operator_precedence)</br>
is a table showing what comes before what.

### Source files an binaries

Exploring the server gives us a few files.

	$ ls
	flag, mistake, mistake.c, password

We have a C source and a binary, in addition to a password file.

#### Understanding the source

Let's dive into the source code.

First, the program tries to open the password file. If the file doesn't exist or can't</br>
open from some reason, the program exits.

	int fd;
	if(fd=open("/home/mistake/password",O_RDONLY,0400) < 0){
		printf("can't open password %d\n", fd);
		return 0;
	}

Then, to prevent bruteforcing, the program sleeps for a number of seconds. This number</br>
is based on the current time, but for the sake of this challenge, we can say it will</br>
be maximum 20 seconds.

	sleep(time(0)%20);

Afterwards, the program reads 10 bytes from the password file (The number of bytes is</br>
defined as `PW_LEN`). If any error occur while reading, the program exits.

	char pw_buf[PW_LEN+1];
	int len;
	if(!(len=read(fd,pw_buf,PW_LEN) > 0)){
		printf("read error\n");
		close(fd);
		return 0;		
	}

Now the program scans 10 bytes from the user.

	char pw_buf2[PW_LEN+1];
	printf("input password : ");
	scanf("%10s", pw_buf2);

Xor the 10 bytes using the `xor` function.

	xor(pw_buf2, 10);

And eventually, compares the two buffers. If matches, we get the flag.

	if(!strncmp(pw_buf, pw_buf2, PW_LEN)){
		printf("Password OK\n");
		system("/bin/cat flag\n");
	}

### Identifying a target

We can understand from the source code that our target is to find a password that</br>
after being processed through the `xor` function, will be equal to the password read</br>
from the password file by the program.

### Identifying interactions

The program receive data in 2 ways, input from the user and input from a file,</br>
identified by the `fd` variable. We can only control the input from the user, since</br>
we don't have access to read/write to the password file.

## Solving the challenge

We got a hint about operator priority.. Let's try and see where it can be used to</br>
our advantage.

There are a few lines in the source code that use not-so-traditional operators (or</br>
atleast unusual usage of operators).

One example looks specifically suspicious:

	s[i] ^= XORKEY;

But if we break it down, there is nothing wrong with the usage of operators in this</br>
line. Top priority will be the `[]` operator, and then `^=`.

Another example will be the asignment of a value into a variable, inside some of the</br>
if statements present in the source code:

	if (fd=open("/home/mistake/password",O_RDONLY,0400) < 0)

Top priority will be the `open(...)` function call, then the `<` boolean operator,</br>
and laslty, the `=` asignment operator.

As written in the Notes section of [this page](https://en.cppreference.com/w/c/language/operator_precedence):

>Operators that are in the same cell (there may be several rows of operators listed in a
>
>cell) are evaluated with the same precedence, in the given direction. For example, the
>
>expression `a=b=c` is parsed as `a=(b=c)`, and not as `(a=b)=c` because of right-to-left
>
>associativity.

The condition is `fd=open(...) < 0`, and will be parsed as `fd=(open(...)<0)`. That</br>
means the variable `fd` will have a value of either 1 or 0 (the result of the boolean</br>
expression). We already know that the file descriptors 1 and 0 are saved for `stdout`</br>
and `stdin`. Therefore, we can potentially set the program to read the password from</br>
`stdin` instead of the password file. We can potentially control the password!

With this in mind, all we need to do is construct 2 passwords. When processing the</br>
second password through the `xor` function, we need it to be equal to the first password.

*we also need them both to be 10 bytes long*.

Looking again at the `xor` function, we see all it does is xor each byte of a given</br>
string by 1. That means, if we send it a bunch of `A`s, it will process each char</br>
to `@`, the result of `65 ^ 1` (65 being the ASCII value of `A`).

Great! we now have the two passwords we needed. Let's get our flag!