# input

***Mom? how can I pass my input to a computer program?***

## Recon

This challenge gives us a connection to the server via ssh:

	ssh input2@pwnable.kr -p2222 (pw:guest)

### Source files and binaries

Let's explore what we have on the server.

	$ ls
	flag, input, input.c

We have a C source and a binary.

#### Understanding the source

This challenge is not some hack or an exploit, it's just giving the correct inputs</br>
in the correct way and order, then we get the flag.

There are 5 stages, each expects different inputs in different ways.

1. *argv*
	
	This stage expects input as command line arguments. Let's see what it needs.

		if(argc != 100) return 0;
		if(strcmp(argv['A'],"\x00")) return 0;
		if(strcmp(argv['B'],"\x20\x0a\x0d")) return 0;

	First, it requires 100 command line arguments. Then, the argument indexed 'A'</br>
	(or the 65th argument) needs to be equal to `"\x00"`, and the argument indexed 'B'</br>
	(or the 66th argument) needs to be equal to `"\x20\x0a\x0d"`.

1. *stdio*

	This stage expects input from the user. Let's see what it needs.

		char buf[4];
		read(0, buf, 4);
		if(memcmp(buf, "\x00\x0a\x00\xff", 4)) return 0;
		read(2, buf, 4);
		if(memcmp(buf, "\x00\x0a\x02\xff", 4)) return 0;

	First, it reads 4 bytes from `stdin`, which are required to be equal to</br>
	`"\x00\x0a\x00\xff"`. Then, it reads 4 more bytes from `stderr`, which are</br>
	required to be equal to `"\x00\x0a\x02\xff"`.

1. *env*

	This stage expects input from the environment. Let's see what it needs.

		if(strcmp("\xca\xfe\xba\xbe", getenv("\xde\xad\xbe\xef"))) return 0;

	It reads an environment variable called `"\xde\xad\xbe\xef"` and compares its</br>
	value to `"\xca\xfe\xba\xbe"`.

1. *file*

	This stage expects input from a file. Let's see what it needs.

		FILE* fp = fopen("\x0a", "r");
		if(!fp) return 0;
		if( fread(buf, 4, 1, fp)!=1 ) return 0;
		if( memcmp(buf, "\x00\x00\x00\x00", 4) ) return 0;

	First, it opens a file called `"\x0a"`. Then, it reads 4 bytes from it and</br>
	compares them to `"\x00\x00\x00\x00"`.

1. *network*

	This stage expects input from a socket through the network. Let's see what it needs.

	First thing first, it opens a TCP/IP socket.

		int sd, cd;
		struct sockaddr_in saddr, caddr;
		sd = socket(AF_INET, SOCK_STREAM, 0);
		if(sd == -1){
			printf("socket error, tell admin\n");
			return 0;
		}

	Then, it binds the socket to a port number given by us, using the argument</br>
	indexed 'C' (or the 67th argument) in the command line arguments list. After</br>
	that, it starts listening for incoming traffic.

		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = INADDR_ANY;
		saddr.sin_port = htons( atoi(argv['C']) );
		if(bind(sd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
			printf("bind error, use another port\n");
    			return 1;
		}
		listen(sd, 1);

	Once such traffic arrives, it forwards it to a different socket.

		int c = sizeof(struct sockaddr_in);
		cd = accept(sd, (struct sockaddr *)&caddr, (socklen_t*)&c);

	Finaly, it reads 4 bytes from the socket and comares to `"\xde\xad\xbe\xef"`.

If all requirements are met, the program then prints the flag.

	system("/bin/cat flag");

### Identifying a target

We have several targets here, for each stage:

1. *argv* - Construct an array of 100 strings. All are junk strings except for the</br>
65th, 66th and 67th elements. They will have some special values:
	
	> arr[65] = `"\x00"`
> 
> arr[66] = `"\x20\x0a\x0d"`
> 
> arr[67] = `Port X`

*Port X will be the port we will use in stage 5.*

2. *stdin* - Send `"\x00\x0a\x00\xff"` to `stdin`, then send `"\x00\x0a\x02\xff"`</br>
to `stderr`.

3. *env* - Export an environment variable `"\xde\xad\xbe\ef"` with the value</br>
`"\xca\xfe\xba\xbe"`.

4. *file* - Create a file `"\x0a"` containing the value `"\x00\x00\x00\x00"`

5. *network* - Connect via a socket to port `Port X` and send through it 4 bytes</br>
with the value `"\xde\xad\xbe\xef"`.

### Identifying interactions

There is not much to say here, it is not a pwn challenge, so there is no need discuss</br>
interactions and execution flow. We've already discussed in depth about the program's</br>
interaction points in two sections already.

## Solving the challenge

For this challenge specifically, it will be easier to write the solution in C instead</br>
of Python. Also, we will run this remotely on the server, and not on our machine.

For stage 1, we will construct an array of 100 strings, + 1 more for a space at the</br>
end. Then, we will set the 65th element to `"\x00"`, the 66th element to `"\x20\x0a\x0d"`</br>
and the 67th element to `Port X`, that we will decide of a value for it later in</br>
stage 5.

Stage 2 is a bit tricky. To solve it, we need to send data to `stdin` and `stderr`.</br>
To do that, we need to pipe `stdin` and `stderr` and write to the pipes.</br>
We will open pipes for `stdin` and `stderr`, use the `fork` function to fork our</br>
process and then use the child process to write to one end of the pipes, so the</br>
parent process can duplicate these pipes to their corresponding `stdin`/`stderr` file</br>
descriptors.

Stage 3 is much easier than stage 2. All we need to do is to set an environment</br>
variable using the `setenv` function, then pass the `environ` variable (an external</br>
variable in the program) when executing the `input` program and it will have our</br>
custom environment.

For stage 4, all we need is to create a file called `"\x0a"` and write 4 NULL bytes</br>
into it. Easy!

For stage 5, the biggest stage of this challenge, we will need to create a socket,</br>
connect with it to `Port X`, and send some data through it. Let's first decide on</br>
the value of `Port X`. This doesn't matter at all, but I've set it to be `8224`</br>
just because, why not?</br>
So, we'll create a socket with the `socket` function, with `AF_INET` and`SOCK_STREAM`</br>
as parameters, to create a TCP/IPv4 socket. Since there is not protocal and we're </br>
just sending raw data, the third parameter - indicating the protocol - will be NULL.</br>
After creating the socket, we will connect to IP address `127.0.0.1` (localhost),</br>
on `Port X` (which we decided for it to be `8224`). Then, last step remaining to</br>
do is to send `"\xde\xad\xbe\xef"` through the socket, and wait for the flag!

## Notes

- This challenge's solution was written in C and not Python, so I commented it a lot</br>
more for you to understand it with ease.
- This challenge's solution doesn't work remotely like most of the other challenges.</br>
For it to work, it needs to be executed on the server machine. We, as challengers,</br>
can create files in `/tmp` on the server. So, this is what we need to do:

***(connected to the server via ssh)***

	$ cd /tmp
	$ mkdir *yourname*
	$ cd *yourname*
	$ # copy the solution file into this directory.

Now, for the last step, we need to create a link to the flag file, since it is no</br>
longer in our working directory. To do this, simply enter the command

	$ ln -sf /home/input2/flag flag

Now, we can just run our solution and get the flag!