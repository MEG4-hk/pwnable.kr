# fd

***Mommy! what is a file descriptor in Linux?***

## Recon

We are given 2 things:

1. An obvious hint -> this challenge is about Linux file descriptors.
1. Connectionto the challenge's machine -> `ssh fd@pwnable.kr -p222 (pw:guest)`

If we are not familiar with file descriptors, with a quick google search we can</br>
understand that a file descriptor is a number we use to identify a file resource.

### Source files and binaries

After connecting to the server via ssh, let's see what we can do:

	$ ls
	fd, fd.c, flag

We have a C source and a binary.

#### Explaining the source

Looking at the source file we have a program that recieves 1 command line argument.

	if(argc<2){
		printf("pass argv[1] a number\n");
		return 0;
	}

The program uses this argument as an integer number, then subtracts 0x1234.

	int fd = atoi( argv[1] ) - 0x1234;

Later, the result is used by the `read` function (actually, syscall) to read 32</br>
bytes from the file identified by the file descriptor, into a 32 bye long buffer</br>
named `buf`, which is declared as a global variable at the start of the program.

	len = read(fd, buf, 32);

At the end of the program, it compares the content of the buffer (or the content</br>
of the file that was read into the buffer earlier) to "LETMEWIN\n". If equals, we</br>
get the flag as a reward :)

	if(!strcmp("LETMEWIN\n", buf)){
		printf("good job :)\n");
		system("/bin/cat flag");
		exit(0);
	}

### Identifying a target

From our understanding of the source code, we need to give the program a number</br>
as the command line argument that - after subtracting `0x1234` - will be a valid</br>
file descriptor of a file with the content `"LETMEWIN\n"`.

### Identifying interactions

We have 2 ways of affecting the execution of the program:

1. Through the command line argument the program recieves.
2. Through the content of the file the program reads.

## Solving the challenge

The easiest way to give the program the content we want it to recieve (`"LETMEWIN\n"`)</br>
is by giving it through user input. But, there's a problem: the program doesn't</br>
read any input from the user!

Combining our knowledge of file descriptors in Linux, we know that the file descriptor</br>
of `stdin` - the file used for user input - is `0`. Thus, we need the `fd` variable to be `0`.

Since the `fd` variable is equal to `argv[1] - 0x1234`, we need our argument to be</br>
equal to `0x1234` (`4660` in base 10). After that, we can just write `"LETMEWIN"` and get</br>
our flag as a result. So, we send `"4660"` as the argument, then type `"LETMEWIN\n".`