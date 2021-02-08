# blackjack

***Hey! check out this C implementation of blackjack game!</br>
I found it [online](http://cboard.cprogramming.com/c-programming/114023-simple-blackjack-program.html)</br>
I like to give my flags to millionares.</br>
how much money you got?***

## Recon

With this challenge, we get:

1. The source code from a post on [cprogramming](http://cboard.cprogramming.com/).
1. A server connection where the game is running remotely -> `nc pwnable.kr 9009`

### Source files an binaries

The game has a single C source file posted online. We will download it to have</br>
a local copy.

#### Understanding the source

Let's have a look at the source code.

This is a very long source file, implementing the entire game in almost **800**</br>
lines of code! We will skip through a lot of functions that are unnecessary for</br>
this challenge.

Looking at the list of functions:

```c
//Function Prototypes
int clubcard();      //Displays Club Card Image
int diamondcard();   //Displays Diamond Card Image
int heartcard();     //Displays Heart Card Image
int spadecard();     //Displays Spade Card Image
int randcard();      //Generates random card
int betting();       //Asks user amount to bet
void asktitle();     //Asks user to continue
void rules();        //Prints "Rules of Vlad's Blackjack" menu
void play();         //Plays game
void dealer();       //Function to play for dealer AI
void stay();         //Function for when user selects 'Stay'
void cash_test();    //Test for if user has cash remaining in purse
void askover();      //Asks if user wants to continue playing
void fileresults();  //Prints results into a file in program directory
```

With basic understanding of a real blackjack game, we can deduct that the only</br>
functions we should pay attention to (in an attacker's perspective) are as follows:

```c
int randcard();      //Generates random card
int betting();       //Asks user amount to bet
void play();         //Plays game
void dealer();       //Function to play for dealer AI
void stay();         //Function for when user selects 'Stay'
void cash_test();    //Test for if user has cash remaining in purse
```

So we will only break down these functions. But if you're still interested, you</br>
can view the rest of the source code yourself - it's mostly user interface and</br>
flow control.

the `randcard` function have caught my attention first. What it does, is, pick</br>
one of 4 types of cards, clubs, diamonds, hearts and spades, then use the</br>
corresponding function to print a card of random value of this type.

```c
random_card = rand()%4+1;
```

Next, the `play` function tells us a bit more about the flow of the program. It</br>
defines the logic of the actual blackjack game.

At the beggining, it tests if we (as players) have enough money to play, using</br>
very high level arithmetic techniques:

```c
cash = cash;
```

And afterwards, uses the `cash_test` function to check if teh player is has any</br>
money left.

```c
if (cash <= 0) //Once user has zero remaining cash, game ends and prompts user to play again
{
	printf("You Are Bankrupt. Game Over");
	cash = 500;
	askover();
}
```

Then, the `play` function draws a random card, calculates the player's total,</br>
then the dealer's total, and calls the `betting` function - we will read this</br>
function later.

```c
randcard(); //Generates random card
player_total = p + l; //Computes player total
p = player_total;
...
dealer(); //Computes and prints dealer total
betting(); //Prompts user to enter bet amount
```

Now, the program uses the player's total cards value to decide what to do next.</br>
There are 3 options.

1. The player's total is equal to `21`:

	The player has won! Print a message, add the bet to the cash, reset some</br>variables and start again.

	```c
	printf("\nUnbelievable! You Win!\n");
	won = won+1;
	cash = cash+bet;
	printf("\nYou have %d Wins and %d Losses. Awesome!\n", won, loss);
	dealer_total=0;
	askover();
	```

1. The player's total is over `21`:

	Busted! The player lost. Same as the first option, print a message, sub</br>
	the bet from the cash, reset some variables and start again.

	```c
	printf("\nWoah Buddy, You Went WAY over.\n");
	loss = loss+1;
	cash = cash - bet;
	printf("\nYou have %d Wins and %d Losses. Awesome!\n", won, loss);
	dealer_total=0;
	askover();
	```

1. The player's total is (you guessed it!) under `21`:

	The game continues! Ask the player whether he wants to hit or stay (sadly</br>
	there is no double down).

	```c
	printf("Please Enter H to Hit or S to Stay.\n");
	scanf("%c",&choice3);
	```

	If the player chose to hit, same routine as earlier - a random card is</br>
	drawn, the total is being calculated (for player and dealer), and the</br>
	program checks if the dealer has won by this time.

	```c
	randcard();
	player_total = p + l;
	p = player_total;
	...
	dealer();
	if(dealer_total==21) //Is dealer total is 21, loss
	{
		...
	}
	if(dealer_total>21) //If dealer total is over 21, win
	{                      
		...
	}
	```

	If the player chose to stay, the `stay` function - which we will discuss in</br>
	a few moments - is being called.

	```c
	printf("\nYou Have Chosen to Stay at %d. Wise Decision!\n", player_total);
	stay();
	```

Wow that was a huge function.</br>
Now we move on to look at the dealer's logic - the `dealer` function.

Huh. Not as interesting as I thought it would be. Basically, as long as the</br>
dealer has less than `17` in total, it will continue drawing random cards.

```c
if(dealer_total<17)
{
	...
}
```

The `stay` function is a bit complicated. It uses recursion to get the dealer's</br>
total to be at least `17`, then it checks who has won the game.

If the player's total equals to or greater than the dealer's total, the player won.

```c
if(player_total>=dealer_total) //If player's total is more than dealer's total, win
{
	won = won+1;
	cash = cash+bet;
	...
	dealer_total=0;
	askover();
}
```

If the player's total is less than the dealer's total, the player lost.

```c
if(player_total<dealer_total) //If player's total is less than dealer's total, loss
{
	loss = loss+1;
	cash = cash - bet;
	...
	dealer_total=0;
	askover();
}
```

Finally, if the dealer's total is over `21`, the player won.

```c
if(dealer_total>21) //If dealer's total is more than 21, win
{
	won = won+1;
	cash = cash+bet;
	...
	dealer_total=0;
	askover();
}
```

At last, the `betting` function.

First, the program asks the user to input an integer value of cash to bet on</br>
this current round.

```c
printf("\n\nEnter Bet: $");
scanf("%d", &bet);
```

Then, the program checks if the user tried to bet more cash then he actually</br>
have - *and prints a message telling him that he is a big dummy* - and asks</br>
for a new bet, then returns it.

```c
if (bet > cash) //If player tries to bet more money than player has
{
	printf("\nYou cannot bet more money than you have.");
	printf("\nEnter Bet: ");
	scanf("%d", &bet);
	return bet;
}
else return bet;
```

### Identifying a target

Our target, as the challenge's MOTD says, is to be be millionares. Easy!

### Identifying interactions

This program has a lot of interactions, all through the user input.

* *Start screen interaction (Y/N)*</br>
	We will always enter Y.

* *Main menu interaction (1/2/3)*</br>
	We will always enter 1

* *Betting interaction (any integer value)*</br>
	This is the vulnerable part, where we can control the bet value.

* *Game action interaction (H/S)*</br>
	This one depends on how we approach this challenge.

* *Play again (another round) interaction (Y/N)*</br>
	We will only need 1 round, so always enter N.

* *Play again (restart game) interaction (Y/N)*</br>
	We will never need to restart the game, so always enter N.

## Solving the challenge

After reading the source code, we can recognize 2 possible approaches, each</br>
exploiting a different part of the program. I'll explain both, but will only</br>
use one of them in our exploit, the one that works 100% of tehe time.

1. **Exploiting the betting system**:

	In the `betting` function, we saw that he program checks whether the bet</br>
	value is more than the total amount of cash we have. The problem with the</br>
	way this is being checked, is that the check only occurs once. That means,</br>
	if we enter a number larger than the amount of cash we have once, the</br>
	program catches it and asks us to input another bet. But if we do it again,</br>
	this time the entered value is not getting checked, so we can potentially</br>
	bet way more than we have, let's say, exactly `1,000,000`, the amount we</br>
	need to have in order to get the flag. After inputing a large number for</br>
	the second time, all we need to do is win the round and the challenge is</br>
	complete!

1. **Exploiting the win/lose system**:

	Many times all over the source code, the program checks if the game is</br>
	already over. If it finds that the player has lost, it code executed</br>
	looks like this:

	```c
	loss = loss+1;
	cash = cash - bet;
	...
	dealer_total=0;
	```

	The interesting line is the second line, which subtracts the `bet` amount</br>
	from the global `cash` variable holding the player's total cash. The type</br>
	of the `bet` variable is `int`, and holds a numeric value, that can be 0,</br>
	possitive, *and negative*. This means, we can potentially bet a negative</br>
	amount of cash, and if we lose (which is very easy to do..), the program</br>
	will subtract a negative value from `cash`, and actually *increase* its</br>
	value! Let's say we bet `-1,000,000`, and immediately select to stay. The</br>
	dealer will continue playing, and will always win, thus we will lose every</br>
	time, always, increasing the amount of cash we have. Challenge is complete!