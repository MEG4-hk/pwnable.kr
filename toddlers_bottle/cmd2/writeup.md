# cmd2

***MOTD***

## Recon

This challenge includes only a connection to the server. This time, without the</br>
password.

Server connection -> `ssh cmd2@pwnable.kr -p2222 (pw:flag of cmd1)`

The password (the flag result of cmd1 challenge) is in [Notes](#Notes)

### Source files an binaries

Similar to cmd1 challenge, the server's directory looks the same.

```bash
$ ls
cmd2  cmd2.c  flag
```

We have a C source and a binary.

#### Understanding the source

The source of this challenge looks very similar to the cmd1 challenge's source,</br>
but a bit more expanded.

The first big difference is that at the start of `main`, there is a new function</br>
that is being called - the `delete_env` function.

Looking inside this function, we can see that all it does is reset the value of</br>
all environment variables.

```c
char** p;
for(p=environ; *p; p++)	memset(*p, 0, strlen(*p));
```

Then, like in cmd1, the program exports an environment variable called PATH with</br>
the value of `'/no_command_execution_until_you_become_a_hacker'`.

The rest of the program is the same as cmd1, except for the list of words not</br>allowed in the `filter` function.

```c
putenv("PATH=/no_command_execution_until_you_become_a_hacker");
if(filter(argv[1])) return 0;
printf("%s\n", argv[1]);
system( argv[1] );
```

Let's have a look at the new list of words we can't use:

```c
r += strstr(cmd, "=")!=0;
r += strstr(cmd, "PATH")!=0;
r += strstr(cmd, "export")!=0;
r += strstr(cmd, "/")!=0;
r += strstr(cmd, "`")!=0;
r += strstr(cmd, "flag")!=0;
```

Now we can't use the words (or symbols) `=`, `PATH`, `export`, `/`, ``` ` ``` or `flag`.

### Identifying a target

Again, we need to construct a command that does not contain any of the words</br>
listed above, and that will print the flag when executed.

### Identifying interactions

This challenge's interactions are exactly as in cmd1, through the first command-line</br>argument given to the prorgram.

## Solving the challenge

The main problem we have on this challenge is that one of the symbols we cannot</br>
use is the `/` symbol, which we need to use in order to print the flag. We need</br>
to think of an alternative, or a way to insert it into our command.

There is an easy solution, similar to what I did in cmd1, but a bit different.

We can use the command `command -p` to execute the `cat` utility on the flag file.</br>
The solution can be as easy as that!

## Notes

ssh password: `mommy now I get what PATH environment is for :)`