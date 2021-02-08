# cmd1

***Mommy! what is PATH environment in Linux?***

## Recon

This challenge includes only the server connection.

Server connection -> `ssh cmd1@pwnable.kr -p2222 (pw:guest)`

#### Understanding concepts

This challenge wants us to try and solve it using something about the PATH</br>
evnironment variable, but I've found a much easier and better way to solve this.

### Source files an binaries

Exploring the server's directory, we can find a few files:

```bash
$ ls
flag  cmd1  cmd1.c
```

We have a C source and a binary.

#### Understanding the source

The source file is very simple.

First, it exports an environment variable named `PATH`. It's value should be</br>
`'/thankyouverymuch'`.

```c
putenv("PATH=/thankyouverymuch");
```

Then, it calls the `filter` function with the first command-line argument. If</br>
the `filter` function returned any value other than 0, the program exits.

```c
if(filter(argv[1])) return 0;
```

Eventually, it uses the filtered argument as a system command and executes it</br>
with the `system` function.

```c
system( argv[1] );
```

Looking inside the `filter` function, it's purpose is to filter some words from</br>
the command argument.

```c
r += strstr(cmd, "flag")!=0;
r += strstr(cmd, "sh")!=0;
r += strstr(cmd, "tmp")!=0;
```

As we can understand, we are not allowed to pass a command containing the words</br>
`flag`, `sh` or `tmp`.

### Identifying a target

Our target is to construct a command to print out the `flag` file wihtout using</br>
any of the forbidden words.

### Identifying interactions

Our way of interacting with the program is via the command line argument we pass</br>
into it.

## Solving the challenge

As mentioned earlier, the challenge expects us to use the PATH environmental</br>
variable to solve it, but we don't actually need to do it that way.

We can enter any command we want, as long as it doesn't contain one of the</br>
words llisted above. But, what we *can* use are *parts* of these words.

the `/bin/cat` utility can get a partial file name ending with a `*` character</br>
and it will automatically search for a matching file in the current directory!

Therefore, we don't need to use the actual word `flag`, only the letter `f` and</br>
a `*` character at the end!