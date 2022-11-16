#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

// Given struct Grid from Project Document
struct Grid {
    char data;
    struct Grid * up;
    struct Grid * down;
    struct Grid * left;
    struct Grid * right;
    struct Grid * upLeft;
    struct Grid * upRight;
    struct Grid * downLeft;
    struct Grid * downRight;
};

// struct Penguin stores data to be used globally for the player & the opponent AI
struct Penguin {
    int score;
    char * scoreStr;
    char name;
    
    int cur_row;
    int cur_column;

    int cur_index;

    int can_move;
};

// function that returns a pointer to a malloc heap, with 36 grid space (acting like an array)
struct Grid* get_board_ptr() {

    return (struct Grid*) malloc(36 * sizeof(struct Grid));
}

// prints the board in a 6x6 manner
void print_board(struct Grid* ptr) {
    for (int i = 0; i < 36; i++) {
        printf("%c ", ptr[i].data);
        if (i % 6 == 5) {
            printf("\n");
        }
    }
}

void setup_board(struct Grid* ptr) {

    /*
        0 1 2 3 4 5
        0 1 2 3 4 5
        0 1 2 3 4 5
        0 1 2 3 4 5
        0 1 2 3 4 5
        0 1 2 3 4 5

        first two rows w/ actual indexes...
         0 1 2 3  4  5
         6 7 8 9 10 11
         ...

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
    */

   // Set up data values in spaces. See above logic.
   for (int i = 0; i < 36; i++) {
       if ((i % 6 == 0 || i % 6 == 5) || (i < 5 || i > 30)) {
           ptr[i].data = '1';
       } else if ((i % 6 == 2 || i % 6 == 3) && (i > 12 && i < 24)) {
           ptr[i].data = '3';
       } else {
           ptr[i].data = '2';
       }
   }

    // Set up pointers to 8 directions. See above logic.
   for (int j = 0; j < 36; j++) {
       if (j > 5) {
           ptr[j].up = &ptr[j-6];
           if (j % 6 != 0) {
               ptr[j].upLeft = &ptr[j-7];
           } 
           if (j % 6 != 6) {
               ptr[j].upRight = &ptr[j-5];
           }
       }
       if (j < 30) {
           ptr[j].down = &ptr[j+6];
           if (j % 6 != 0) {
               ptr[j].downLeft = &ptr[j+5];
           }
           if (j % 6 != 6) {
               ptr[j].downRight = &ptr[j+7];
           }
       }
       if (j % 6 != 0) {
           ptr[j].left = &ptr[j-1];
       }
       if (j % 6 != 5) {
           ptr[j].right = &ptr[j+1];
       }
   }

   // prints the board to start
   printf("Starting board:\n");
   print_board(ptr);
}




int translate_to_index(int row, int column) {
    // board is a straight series of positions, 0-35, takes in a row and column to translate to an index position
    
    return ((row - 1) * 6) + (column - 1);
}

void choose_start(struct Grid *ptr, struct Penguin *ptr_player) {
    // get a starting position. works with both the player and opponent AI
    int startingRow, startingColumn, startingIndex;

    //get random seed for position for AI on start, based on the time
    srand(time(0));

    while (1) {
        // if you're the player, get from console
        if (ptr_player->name == 'P') {
            printf("Enter starting row and column with space in-between: ");
            scanf("%d %d", &startingRow, &startingColumn);
        
        // if not, choose the position randomly until you get a 1
        } else {
            startingRow = rand() % 6;
            startingColumn = rand() % 6;
        }
            startingIndex = translate_to_index(startingRow, startingColumn);
        
        if (ptr[startingIndex].data != '1') {
            if (ptr_player->name == 'P') {
                printf("Selected grid does not have value 1, please choose another\n");
            }
            
        } else {
            break;
        }
        
    }

    // set actual position & pointers for starting grid

    printf("%c starts at index %d\n", ptr_player->name, startingIndex);

    ptr[startingIndex].data = ptr_player->name;
    ptr_player->cur_index = startingIndex;
    ptr_player->score += 1;

    ptr_player->cur_row = startingRow;
    ptr_player->cur_column = startingColumn;
}

int check_dir (int temp_index, int cur_index, struct Grid *ptr) {
    // check all 8 directions around inputted player and see if there's an opening
    // dir_tracker is the "score" of how many openings. If 0, you're trapped

    int dir_tracker = 0;
    for (int r = 0; r < 8; r++) {
        if (r == 0) {
            temp_index -= 6;
            if (temp_index >= 0 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 1) {
            temp_index += 6;
            if (temp_index < 35 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 2) {
            temp_index -= 1;
            if (temp_index % 6 != 5 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 3) {
            temp_index += 1;
            if (temp_index % 6 != 0 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 4) {
            temp_index -= 7;
            if (temp_index % 6 != 5 && temp_index >= 0 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 5) {
            temp_index -= 5;
            if (temp_index % 6 != 0 && temp_index >= 0 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 6) {
            temp_index += 5;
            if (temp_index % 6 != 5 && temp_index <= 35 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } if (r == 7) {
            temp_index += 7;
            if (temp_index % 6 != 0 && temp_index <= 35 && ptr[temp_index].data != 'A' && ptr[temp_index].data != '.') {
                dir_tracker += 1;
            }
            temp_index = cur_index;
        } 
        
    }

    return dir_tracker;
}

void move(struct Grid *ptr, struct Penguin *ptr_player) {
    // get scan in format DIRECTION NUMBER
    // ^^ has to be in straight line

    char * direction;
    int magnitude;

    struct Grid * desired_ptr = &ptr[0]; // the next space in the desired direction
    int cur_index = ptr_player->cur_index; // index of current grid
    struct Grid cur_grid = ptr[cur_index]; // grid object of where player is currently

    struct Grid * temp = &cur_grid; //copy which points to the current grid position

    int temp_index = cur_index;
    int dir_tracker = 0;

    // if you're the player:
    while (ptr_player->name == 'P' && ptr_player->can_move == 1) {
        // get # of openings around the player
        dir_tracker = check_dir(temp_index, cur_index, ptr);     

        if (dir_tracker == 0) {
            printf("Player has no where to go\n");
            ptr_player->can_move = 0;
            // pull out of loop to see if there's a draw
            break;

        }

        // get a move in the form DIRECTION NUMBER
        printf("%c, Enter move: ", ptr_player->name);
        scanf("%s %d", &direction, &magnitude);

        /*
            The logic for everything below is to:
            1.) Compare the entered string to a direction, like UP, DOWN, etc.
            2.) If it matches, loop as many times as the magnitude entered, and...
            3.) Change the index...
            4.) Check if index out of bounds of the board. If so, wipe and start over
            5.) If not out of bounds, move the pointer to the next grid position down in the requested direction.
            If that space is occupied by the opposite player OR is a space, wipe and start over
            6.) If everything's fine from above, set you're go-to pointer to the one reached

            Essentially just goes in one direction as requested, checking for roadblocks.
            If it hits one, start over and pick another move.
        */

        if (strcmp(&direction, "UP") == 0) {
            for (int i = 0; i < magnitude; i++) {       
                cur_index -= 6;
                if (cur_index < 0) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->up;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "DOWN") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index += 6;
                if (cur_index > 35) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->down;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "LEFT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index -= 1;
                 if (cur_index % 6 == 5) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->left;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "RIGHT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index += 1;
                if (cur_index % 6 == 0) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->right;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "UPLEFT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index -= 7;
                if (cur_index % 6 == 5 || cur_index < 0) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->upLeft;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "UPRIGHT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index -= 5;
                if (cur_index % 6 == 0 || cur_index < 0) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->upRight;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "DOWNLEFT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index += 5;
                if (cur_index % 6 == 5 || cur_index > 35) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->downLeft;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else if (strcmp(&direction, "DOWNRIGHT") == 0) {
            for (int i = 0; i < magnitude; i++) {
                cur_index += 7;
                if (cur_index % 6 == 0 || cur_index > 35) {
                    printf("Path goes out of bounds, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
                temp = temp->downRight;
                if (temp->data == 'A' || temp->data == '.') {
                    printf("No path to desired node, please move again \n");
                    temp = &cur_grid;
                    cur_index = ptr_player->cur_index;
                    break;
                }
            }
            desired_ptr = temp;
            break;
        } else {
            printf("Entered incorrect direction, try again: \n");
        }

        
    }
    
    
    // if you're the AI...
    if (ptr_player->name == 'A' && ptr_player->can_move == 1) {


        /*
            UP = 0
            DOWN = 1
            LEFT = 2
            RIGHT = 3
            UPLEFT = 4
            UPRIGHT = 5
            DOWNLEFT = 6
            DOWNRIGHT = 7
        */

       int max_score = 0;
       int tracked_score = 0;
       int max_index = 0;
       int tracked_index = 0;
       struct Grid * max_pointer;

       /*
            Works every similarly to the player logic above, but looks for the maximal
            path. Loops through all directions around the current position and all possible
            magnitudes, looking for the ideal position to move to. Compares after each
            checked path to see if it's the maximal one. If so, save.

       */

        // get # of openings around the AI
        dir_tracker = check_dir(temp_index, cur_index, ptr);     

        if (dir_tracker == 0) {
            printf("AI has no where to go\n");
            ptr_player->can_move = 0;
            return;

        }

        for (int direction2 = 0; direction2 < 8; direction2++) {

            for (int magnitude2 = 1; magnitude2 < 6; magnitude2++) {

                tracked_score = 0;

                if (direction2 == 0) {
                    for (int i = 0; i < magnitude2; i++) {       
                        cur_index -= 6;
                        if (cur_index < 0) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->up;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 1) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index += 6;
                        if (cur_index > 35) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->down;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 2) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index -= 1;
                        if (cur_index % 6 == 5) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->left;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 3) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index += 1;
                        if (cur_index % 6 == 0) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->right;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 4) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index -= 7;
                        if (cur_index % 6 == 5 || cur_index < 0) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->upLeft;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 5) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index -= 5;
                        if (cur_index % 6 == 0 || cur_index < 0) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->upRight;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 6) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index += 5;
                        if (cur_index % 6 == 5 || cur_index > 35) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->downLeft;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                } else if (direction2 == 7) {
                    for (int i = 0; i < magnitude2; i++) {
                        cur_index += 7;
                        if (cur_index % 6 == 0 || cur_index > 35) {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        temp = temp->downRight;
                        if (temp->data == 'P' || temp->data == '.') {
                            temp = &cur_grid;
                            cur_index = ptr_player->cur_index;
                            break;
                        }
                        if (temp->data > 48 && temp->data < 52) {
                            tracked_score += (int) temp->data - 48;
                        }
                    }
                    desired_ptr = temp;
                    tracked_index = cur_index;
                }

                // do the checking for the max score, and make sure it doesn't pick it's last position as the most ideal
                if (tracked_score > max_score && desired_ptr->data != 'A') {
                    max_score = tracked_score;
                    max_pointer = desired_ptr;
                    max_index = tracked_index;
                }

            }
        }

        desired_ptr = max_pointer;
        cur_index = max_index;
    }

    // for both player & opponent AI, update the positions & scores formally
    if (desired_ptr != &cur_grid) {
        ptr_player->score += (int) desired_ptr->data - 48 ; //increment score
        if (desired_ptr->data != '.') {
            strncat(ptr_player->scoreStr, &desired_ptr->data, 1);
            strncat(ptr_player->scoreStr, "+", 2);
            printf("New index for %c is %d\n", ptr_player->name, cur_index);
            printf("New score for %c is %d\n", ptr_player->name, ptr_player->score);
        }

        ptr[ptr_player->cur_index].data = '.'; //replace old grid with an empty space
        desired_ptr->data = ptr_player->name; //set the new grid position to the player
        ptr_player->cur_index = cur_index; //set the player's index to the new index
    }

}   



int main() {

    // initialize  player and opponent + their pointers & vars
    struct Penguin player, opponent;
    struct Penguin *playerP = &player;
    struct Penguin *opponentP = &opponent;
    struct Penguin *cur_penguin_turn = playerP;

    int game_running = 1;

    // extra strings for doing the end game score string
    char * end_str_player = (char*)malloc(10*sizeof(char));
    char * end_str_opponent = (char*)malloc(10*sizeof(char));
    
    // vars for player & opponent + adding pointer strings for the score strings
    player.score = 0;
    player.scoreStr = (char*)malloc(100*sizeof(char));
    strcpy(player.scoreStr, "1+");
    player.name = 'P';
    player.can_move = 1;

    opponent.score = 0;
    opponent.scoreStr = (char*)malloc(100*sizeof(char));
    strcpy(opponent.scoreStr, "1+");
    opponent.name = 'A';
    opponent.can_move = 1;

    // get the game board pointer to memory heap
    struct Grid* gridP = get_board_ptr();

    // set up the values & pointers of the board
    setup_board(gridP);

    // get the starting positions of the player and opponent
    choose_start(gridP, playerP);
    choose_start(gridP, opponentP);

    // print the board after starts are chosen
    print_board(gridP);

    // main game loop
    while (game_running) {

        // main moving function, based on a pointer of who's turn it is
        move(gridP, cur_penguin_turn);

        // checking for end game states, aka when the players are stuck

        if (player.can_move == 0 && opponent.can_move == 0) {
            printf("Player and AI can't move! Final scores: \n");
           player.scoreStr[strlen(player.scoreStr)-1] = '\0';
            strncat(player.scoreStr, "=", 2);
            opponent.scoreStr[strlen(opponent.scoreStr)-1] = '\0';
            strncat(opponent.scoreStr, "=", 2);
            printf("%c : %s", player.name, player.scoreStr);
            sprintf(end_str_player, "%d", player.score);
            printf("%s\n", end_str_player);
            printf("%c : %s", opponent.name, opponent.scoreStr);
            sprintf(end_str_opponent, "%d", opponent.score);
            printf("%s\n", end_str_opponent);

            if (player.score > opponent.score) {
                printf("Player won!\n");
            } else if (player.score < opponent.score) {
                printf("Opponent won! \n");
            } else {
                printf("Draw! \n");
            }
            break;
        }

        print_board(gridP);

        // logic to switch turns back and forth
        if (cur_penguin_turn == playerP && opponent.can_move == 1) {
            cur_penguin_turn = opponentP;
        } else if (cur_penguin_turn == opponentP && player.can_move == 1){
            cur_penguin_turn = playerP;
        }
    }

    // free the allocated board memory
    free(gridP);
    free(player.scoreStr);
    free(opponent.scoreStr);
    free(end_str_player);
    free(end_str_opponent);

}