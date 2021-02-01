# pwnable.kr

![pwnable.kr logo](/assets/images/logo.png)

## Target

This repository holds my solutions of the challenges presented in [pwnable.kr](https://pwnable.kr).</br>
It is not always the best solution, nor best written.</br>
If you have any comments or feedback, I'm always be happy to learn.</br>

## Dependencies

- python 3.6.9 or above
- `pwntools` python module

## Description

There are 4 "difficulty levels":

- [Toddler's Bottle](/toddlers_bottle)
- [Rookiss](/rookiss)
- [Grotesque](/grotesque)
- [Hacker's Secret](/hackers_secret)

Since I'm a noob at pwn challenges, as of the time I'm writing this README file</br>
I only solved the challenges in Toddler's Bottle, but I will be completing the</br>
rest of the different challenges in the 3 other difficulties as time goes.

## Usage

Each difficulty has its own directory, containing a distinguished direcrotry</br>
for each challenge in it.

Each challenge has its own directory, containint all source files and binaries</br>
given with the challenge, a writeup explaining the solution and a python script</br>
named `exploit.py`, which contains a possible solution for the challenge.

> \* All python scripts were written in python3 unless specifically said otherwise
>
> \* All python scripts require the `pwntools` module to be executed, as mentioned</br>
in the [Dependencies](#Dependencies) section.