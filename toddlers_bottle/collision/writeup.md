# collision

***Daddy told me about cool MD5 hash collision today.</br>
I wanna do something like that too!***

## Recon

We are given 2 things:

1. A hint -> this challenge is about MD5 hash collision.
1. Connection to the challenge's machine -> `ssh col@pwnable.kr -p222 (pw:guest)`

Let's say we do not know what MD5 hash collision is. Together, we can try and</br>
understand that.

#### Understanding concepts

* *MD5* - **M**essage **D**igest algorithm **5** is a cryptographic *hash* function widely used</br>
in the world. It produces a 128-bit hash value.
* *Hash functions* - Hash functions are functions that can digest data of any size</br>
into mapped, fixed-size values. These values are (or suppose to be) irreversible.
* *MD5 hash collision* - The MD5 hash function has several vulnerabilities. one of</br>
them is being exposed to collision attacks, which is finding 2 different values</br>
which prouduce the same output from a hash function.

### Source files an binaries

As usual, we should investigate the server:

	$ ls
	col, col.c, flag

We have a C source and a binary.

#### Understanding the source

Reading the start of the main function, we can understand that the program expects</br>
a single string command line argument, 20 characters long.

	if(argc<2){
		printf("usage : %s [passcode]\n", argv[0]);
		return 0;
	}
	if(strlen(argv[1]) != 20){
		printf("passcode length should be 20 bytes\n");
		return 0;
	}

This argument acts as a password. When entering the correct password, we get our flag.

	if(hashcode == check_password( argv[1] )){
		system("/bin/cat flag");
		return 0;
	}

The function that checks our password is called `check_password`, pretty self explanatory.

This function recieves a string and returns an unsigned long numeric value.

	unsigned long check_password(const char* p)

So, what does this function do?

First, it takes the 20 bytes long string and turns it into an array of integer values.</br>
The 20 characters long string becomes a 5 elements long integer value.

	int* ip = (int*)p;

Then, it sums all the elements into a variable called `res` and returns its value.

	for(i=0; i<5; i++){
		res += ip[i];
	}
	return res;

Going back to the main function, we see that the program checks if the value returned</br>
from `check_password` is equal to a global variable `hashcode`, which is declared above:

	unsigned long hashcode = 0x21DD09EC;

### Identifying a target

After reading the source code, we understand that if the return value from `check_password`</br>
function is equal to `hashcode` (or `0x21DD09EC`), the flag is printed for us.

### Identifying interactions

The program recieves no input at all.</br>
The only interaction we have with the program is through the argument we give it, the</br>password.

## Solving the challenge

Now we know that we need to construct an array of 5 integer values, that when summed,</br>
will give us the value `0x21DD09EC`

to construct such a payload, we can have the first 16 bytes to be set to `0x1`, then</br>
subtract `0x21DD09EC` with what we have, and use the value after the subtraction as</br>
the last 4 bytes of the payload. As a result, we will now have a 20 bytes long payload</br>that when processed through `check_password` function, returns `0x21DD09EC`.

After we calculate our payload, we send it as the argument to the program, and get</br>
the flag.