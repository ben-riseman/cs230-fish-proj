#CS 230 Project 1 - Hey, That's My Fish!
##Implementation by Benjamin Riseman, 2021


***
Implementation of board game "Hey, That's My Fish!" in C. First coding project I did in C.
Given little to no instructions on how to implement the game, so most of this is my attempt at a good design.
May have some minor errors that were hard to crack - it was my first attempt at C programming, after all!
***


1.) Board using pointers:

The game board is implemented using the Grid struct given in the project document.
Essentially, the function get_board_ptr() returns a heap (using malloc) of 36 Grid
objects, numbering 0-35 in index. By iterating through this, all positions can be
reached as if it was a 6 x 6 2D array.

In order to go all 8 directions acting like a board, I had to implement index changes
like so ( as seen in the function setup_board() ) :

	UP: index - 6 if index > 5
        DOWN: index + 6 if index < 30
        LEFT: index - 1 if index % 6 != 0
        RIGHT: index + 1 if index % 6 != 5
        UPLEFT: index - 7 if index % 6 != 0 && index > 5
        UPRIGHT: index - 5  if index % 6 != 6 && index > 5
        DOWNLEFT: index + 5 if index % 6 != 0 && index < 30
        DOWNRIGHT: index + 7 if index % 6 != 6 && index < 30

        data: 1 if index % 6 == 0 || 5 && if index < 5 || index > 30
        data: 3 if index % 6 == 2 || 3 && index > 6*2 && index < 6*4
        data: 2 otherwise (else)

Once this function runs, the board is set up with pointers to adjacent pieces, and
the data (1-3) required for scoring.

2.) Starting the game

I made another struct, called Penguin, to keep track of variables for the player and opponent.
This includes things like their score, current index in the heap, and concatenated string for
the end-game display.

A function choose_start() runs for both the player and opponent, letting the player manually choose
their position and the AI picking a random '1' grid spot.

3.) Game loop

The main driver of the game loop is the move() function, which works for both
the player and AI, with similar logic.

To sum it up, the first thing the function does is call another function, check_dir(), to see how
many adjacent spaces are open. This helps to determine whether or not the game is over for that
particular player.

Next, one of the players goes. By default, the player goes first. Their logic is based on entering
a move, in the format DIRECTION NUMBER, which is compared to preset directions by if-else statements.
For each direction and number (aka magnitude of movement), I needed to check:

	1.) If the player was going to go out of bounds
	2.) If the player was moving into an occupied or empty (already taken) space

If either of those things were true, the loop exits and asks for a re-enter. However, one cannot 
check the pointers reliably, so I checked by index to make sure the index would work (before actually indexing
the board pointer). 

The loop works by checking each space in that requested direction and number, checking each one
before actually moving the player. The same logic applies for the AI, but in a different initial process
than asking for an input.

The AI's logic for picking a space (regardless of whether it's a valid one) comes from a search for a maximal point.
Basically, the AI looks to prolong the game by going as far away from itself as possible (highest magnitude) with
the highest score along the way. The logic behind this is to keep the AI moving away from where it was, and towards
higher-valued node paths, not necessarily seeking closer-to-home high-value nodes. It's not the smartest, but it
works better than moving one space at a time.

At the end of the move() function, necessarily values of the Penguin objects are changed (like score and current pos),
and it returns to the loop. A current turn pointer switches back and forth to tell move() who's actually going
to move.

4.) End logic

If there are no adjacent spaces, a can_move flag in the Penguin objects is set to 0 (or false), making their
move() call do nothing. Once neither player can move, scores are printed using pointer strings that were
accumulated along the way, using strncat(), and strcpy().

5.) Final Notes

I ended up submitting the project a day late because it worked in its final form the night of, but I wanted to
do extra bug fixing and add proper comments + this README file. I decided I would rather take the 10% point hit
than risk many bugs dragging down my score in the long run. 

This was a challenging but rewarding project.

-Benjamin Riseman
