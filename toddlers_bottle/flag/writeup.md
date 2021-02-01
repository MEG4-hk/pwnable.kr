# flag

***Papa brought me a packed present! let's open it.</br>
This is reversing task. all you need is binary***

## Recon

This is a much more minimal challenge, so we do not get a lot of information.

1. A link to download the [flag file](http://pwnable.kr/bin/flag)
1. A note, that this challenge is a reversing task.

### Source files and binaries

No source files are given to us, and there is no server, only a single link</br>
to download the binary.

We have a binary.

### Identifying a target

As this challenge is a reversing task, we need to find a string in the binary</br>
which is the flag itself.

### Identifying interactions

There are no interactions with the program.</br>
Executing the program gives us the following message.

	I will malloc() and strcpy the flag there. take it.

Afterwards, the program exits.

## Solving the challenge

So, first thing first, let's check the type of this binary file.

	$ file src/flag 
	src/flag: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, stripped

This file is an executable. We can set it as a recognized executable with the</br>
`chmod` utility.

	$ chmod +x src/flag

Now let's run the `strings` utility on it to search for the flag.

	$ strings src/flag
	...
	[junk strings]
	..
	UPX!
	UPX!

That's interesting. This binary is compressed with `upx`.</br>
`upx` is a tool that compresses executable binaries and decompresses them on runtime.

We'll install and use `upx` to decompress the binary.

	$ sudo apt install upx
	$ upx -d src/flag
	...
	Unpacked 1 file.

Now when the binary is unpacked, let's try again to get the flag. `strings` will</br>
still return a lot of junk strings, so let's open it using `gdb`.

	$ gdb src/flag

Disassembling the main function, we get some interesting results.

	(gdb) disas main
	Dump of assembler code for function main:
		0x0000000000401164 <+0>:	push   %rbp
		0x0000000000401165 <+1>:	mov    %rsp,%rbp
		0x0000000000401168 <+4>:	sub    $0x10,%rsp
		0x000000000040116c <+8>:	mov    $0x496658,%edi
		0x0000000000401171 <+13>:	callq  0x402080 <puts>
		0x0000000000401176 <+18>:	mov    $0x64,%edi
		0x000000000040117b <+23>:	callq  0x4099d0 <malloc>
		0x0000000000401180 <+28>:	mov    %rax,-0x8(%rbp)
		0x0000000000401184 <+32>:	mov    0x2c0ee5(%rip),%rdx        # 0x6c2070 <flag>
		0x000000000040118b <+39>:	mov    -0x8(%rbp),%rax
		0x000000000040118f <+43>:	mov    %rdx,%rsi
		0x0000000000401192 <+46>:	mov    %rax,%rdi
		0x0000000000401195 <+49>:	callq  0x400320
		0x000000000040119a <+54>:	mov    $0x0,%eax
		0x000000000040119f <+59>:	leaveq 
		0x00000000004011a0 <+60>:	retq   
	End of assembler dump.

We can see a comment that says `0xc2070 <flag>`. We ca assume that it's the address</br>
of the flag!

Examining the address gives us the flag.

	(gdb) x/s *0x6c2070
	0x496628:	"UPX...? sounds like a delivery service :)"

## Notes

The is no `exploit.py` python script since there is no exploit or anything that needs</br>
a script to be performed. Following the writeup in the terminal is enough to solev</br>
this challenge.