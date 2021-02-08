# lotto

***Mommy! I made a lotto program for my homework.</br>
do you want to play?***

## Recon

This challenge gives us only a server connection via ssh:

Server connection -> `ssh lotto@pwnable.kr -p2222 (pw:guest)`

#### Understanding concepts

* */dev/urandom* - A special file in Unix-like system that acts as a (pseudo) random</br>
number generator. The difference between `/dev/urandom` and `/dev/random` is</br>
that `/dev/urandom` does not block coed execution if there is less entropy available</br>
than requested.

### Source files an binaries

Let's look around the server given to us.

```bash
$ ls
flag  lotto  lotto.c
```

We have a C source and a binary.

#### Understanding the source

At the start of the program, a menu will be displayed for us, letting us choose</br>
between 3 options - play, help and exit. Then program will scan our input and</br>
continue the run of the program according to our choice.</br>
*This will repeat endless times until we choose to exit.*

```c
while(1){
	printf("- Select Menu -\n");
	printf("1. Play Lotto\n");
	printf("2. Help\n");
	printf("3. Exit\n");
	scanf("%d", &menu);

	switch(menu){
		case 1:
			play();
			break;
		case 2:
			help();
			break;
		case 3:
			printf("bye\n");
			return 0;
		default:
			printf("invalid menu\n");
			break;
	}
}
```

This is basically all of the main function, so let's now try to understand the</br>
other two functions, `help` and `play`.

The `help` function only prints some information to the screen.

```c
printf("- nLotto Rule -\n");
printf("nlotto is consisted with 6 random natural numbers less than 46\n");
printf("your goal is to match lotto numbers as many as you can\n");
printf("if you win lottery for *1st place*, you will get reward\n");
printf("for more details, follow the link below\n");
printf("http://www.nlotto.co.kr/counsel.do?method=playerGuide#buying_guide01\n\n");
printf("mathematical chance to win this game is known to be 1/8145060.\n");
```

There isn't anything important to us in this function, except maybe the subtle</br>
warning to not try and bruteforce it, since the "mathematical chance to win this</br>
game is known to be 1/8145060". Not good for bruteforcing if we want it to be</br>
done quick.

Now when bruteforcing is off the table, let's have a look at the `play` function.

The program asks us to input 6 "lotto bytes", basically 6 bytes between `1~45`.</br>

```c
int r;
r = read(0, submit, 6);
```

After that, the program generates 6 random numbers by reading 6 bytes from the</br>
pseudo-random number generator file `/dev/urandom`.

```c
int fd = open("/dev/urandom", O_RDONLY);
...
unsigned char lotto[6];
if(read(fd, lotto, 6) != 6){
	... -> handling errors when reading from /dev/urandom
}
```

At this point, the program matches the random numbers to "lotto bytes", and</br>
closes the `/dev/urandom` file.

```c
for(i=0; i<6; i++){
	lotto[i] = (lotto[i] % 45) + 1;		// 1 ~ 45
}
close(fd);
```

To check if our submission matches the generated lotto bytes, the program</br>
itterates through both arrays and if it finds a matching byte, it increments</br>
a counting variable named `match`.

```c
int match = 0, j = 0;
for(i=0; i<6; i++){
	for(j=0; j<6; j++){
		if(lotto[i] == submit[j]){
			match++;
		}
	}
}
```

At the end of the function, it checks if all 6 bytes macthed. If matched, we</br>
get the flag. If not, well, we get another try.

```c
// win!
if(match == 6){
	system("/bin/cat flag");
}
else{
	printf("bad luck...\n");
}
```

### Identifying a target

Our target is for the `match` variable to be equal to 6 at the end of the loop</br>
that compares our submission with the generated lotto bytes.

### Identifying interactions

We can only interact with the program via standard input, when submitting our</br>
lotto bytes.

## Solving the challenge

Let's have another look inside the `play` function, which is probably where we</br>
can find a vulnerability.

If we focus on the comparison loop, we can see that it doesn't work exactly as</br>
intended.

```c
// calculate lotto score
int match = 0, j = 0;
for(i=0; i<6; i++){
	for(j=0; j<6; j++){
		if(lotto[i] == submit[j]){
			match++;
		}
	}
}
```

It itterates through the `lotto` array, and compares each byte to all of the</br>
bytes in the `submit` array, and for every match it increment the `match` variable.

With this in mind, we can understand that if all 6 bytes of the `submit` array</br>
are identical, and matches a single byte from the `lotto` array, the value of</br>
`match` will be 6 after the execution of this loop, exactly what we need. The</br>
only problem now is that we still don't know what bytes are in `lotto`.

To solve this last problem on our way to get the flag, we can use a bruteforce!

Instead of bruteforcing the entire array (which will take \~`8145060` tries), we</br>
only need to bruteforce a single byte in the array, which has only 46 possible</br>
values! Much more feasible. 

Now, there are 46 possible values, 6 bytes, which means the chance of success</br>
is just a bit less than 1/7!

After an easy bruteforce, the challenge is now complete!