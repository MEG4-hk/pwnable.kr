# shellshock

***Mommy, there was a shocking news about bash.</br>
I bet you already know, but lets just make it sure :)***

## Recon

The shellshock challenge comes with a bit of information:

1. A hint in its name -> This challenge is about shellshock exploit in bash
1. Server connection -> `ssh shellshock@pwnable.kr -p2222 (pw:guest)`

#### Understanding concepts

In this section we will explain briefly about new interesting conncepts that are</br>
for this challenge:

* *shellshock* - Shellshock is an exploit discovered in bash around 2014. To use it,</br>
we pass a magic string `() { :;};` as a bash variable when executing a program. Bash</br>
parses it differently than a normal variable, because it has the structure and syntax</br>
of a function. Parsing this variable makes bash execute it and thus execute our code.

### Source files an binaries

Let's check what we have on the server.

	$ ls
	bash  flag  shellshock	shellshock.c

We have a C source and a binary, in addition to a vulnerable version of bash.

#### Understanding the source

This challenge's source code is super minimal.

The program sets its own real, effective and saved user ID to its effective group ID</br>
using the `setresuid` function.

	setresuid(getegid(), getegid(), getegid());

Then, it does the same with the real, effective and saved group ID.

	setresgid(getegid(), getegid(), getegid());

That means, the program will run with the group ID of its owner, `shellshock_pwn`.

Finaly, it starts the vulnerable version of bash with a command to echo the message</br>
`'shock_me'` to the screen.

	system("/home/shellshock/bash -c 'echo shock_me'");

### Identifying a target

Our target is very simple, use the shellshock vulnerability to print the flag ourselves.

## Solving the challenge

One form of the shellshock vulnerability is via environment variables. We can export</br>
any value we want into an environment variable, including a value that starts with the</br>
magic string `() { :;};` and then when we will execute the program, our commands will</br>
be executed as well. This should do it:

	$ export a="() { :;}; /bin/cat flag;"

After that we will execute the program:

	$ ./shellshock

And that's it!