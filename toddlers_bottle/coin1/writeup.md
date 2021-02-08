# coin1

***Mommy, I wanna play a game!***

## Recon

In this challenge all we get is a connection to the running game via netcat:

Connection -> `nc pwnable.kr 9007`

### Source files an binaries

In this challenge we get no sources/binaries, only the game running remotely.

### Identifying a target

Opening the game shows us this welcome message:

	---------------------------------------------------
	-              Shall we play a game?              -
	---------------------------------------------------
	You have given some gold coins in your hand
	however, there is one counterfeit coin among them
	counterfeit coin looks exactly same as real coin
	however, its weight is different from real one
	real coin weighs 10, counterfeit coin weighes 9
	help me to find the counterfeit coin with a scale
	if you find 100 counterfeit coins, you will get reward :)
	FYI, you have 60 seconds.
	- How to play - 
	1. you get a number of coins (N) and number of chances (C)
	2. then you specify a set of index numbers of coins to be weighed
	3. you get the weight information
	4. 2~3 repeats C time, then you give the answer
	- Example -
	[Server] N=4 C=2 	# find counterfeit among 4 coins with 2 trial
	[Client] 0 1 		# weigh first and second coin
	[Server] 20			# scale result : 20
	[Client] 3			# weigh fourth coin
	[Server] 10			# scale result : 10
	[Client] 2 			# counterfeit coin is third!
	[Server] Correct!
	- Ready? starting in 3 sec... -

This means our target is to find the counterfeit coin 100 times under 60 seconds.

## Solving the challenge

This challenge is not actually a pwn challenge, but solvable using mathematics</br>
and algorithms.

We can use binary search to look for the counterfeit coin.

To do so, we weigh half of the coins we have each round. If the total weight of</br>
this half ends with a 0, this means this half does not contain the counterfeit</br>
coin and we can eliminate it completely, thus the other half's size will be our</br>
N for the next round. On the other hand, if the total weight of this half ends</br>
with a 9, this means the counterfeit coin is located in this half.

So, let's start writing a script to solve this. The script will connect to the</br>
game, extract the values of N and C from the given message, then start with the</br>
binary search until it reaches 100 coins, or the 60 seconds timeout.

## Notes

Since most networks (including mine) will have bad connection to the server and</br>
slow response time, we need to run locally. To do that, you will need to upload</br>
the `local_solution.py` file to the pwnable.kr server. Run it there, and it will</br>
work much faster.

Also, there is a very small chance of this script failing. Not sure why, but I</br>
will try to fix it sometime. *maybe*.