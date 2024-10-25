#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// each player has a name and a grid:
typedef struct player Player;
struct player
{
    char name[100]; // reading at most 99 characters for player's name
    int **grid;
};

/*NOTE!!! for the cells of the grid:
empty: int -1, char '*'
miss: int 0, char 'o'
hit: int 1, char '*'
has submarine: int 2
has destroyer: int 3
has battleship: int 4
has carrier: int 5
*/
enum cellStates
{
    empty = -1,
    miss,
    hit,
    submarine,
    destroyer,
    battleship,
    carrier
};

// given an int i (2->5), returns the conrresponding ship's name (only called on i within the specified range)
char *getShipName(int i);

// greate a 10x10 grid, initially empty
int **createGrid();

// display the grid
void displayGrid(Player *player);

// function for placing the ships on the grid
void placeShips(Player *player);

// global variable mode, i.e. tracking difficulty level chosen by the user
int mode;

int main()
{
    srand(time(NULL)); // seed the randon number generator with current time, to ensure different output on different runs

    // game setup and initialization:
    // create player1 and player2, read their names, initialize and display their grids
    // read game "mode", i.e. tracking difficulty level (0 for easy, 1 for hard)

    Player player1;
    Player player2;

    player1.grid = createGrid();
    player2.grid = createGrid();

    printf("Welcome to MAN.BattleShip! Please choose the tracking difficulty level by entering '0' for easy and '1' for hard: ");
    scanf_s("%d", &mode);

    printf("Please enter your names!\nPlayer1: ");
    scanf_s("%99s", player1.name, 100);
    printf("Player2: ");
    scanf_s("%99s", player2.name, 100);

    displayGrid(&player1);
    displayGrid(&player2);

    int startingPlayer = rand() % 2; // 0 for player1, 1 for player2

    Player *A; // pointer to the starting player
    Player *B; // pointer to the other player

    if (startingPlayer == 0)
    { // player1 starts
        A = &player1;
        B = &player2;
    }
    else
    { // player2 starts
        A = &player2;
        B = &player1;
    }

    printf("Placing your ships: \n%s will start!\n", A->name);
    placeShips(A);
    printf("Now is %s's turn!\n", B->name);
    placeShips(B);

    // CONSIDER: adding more conditions to the code so far to handle invalid inputs from the user

    // next: start game

    // free memory allocated for the grids
    for (int i = 0; i < 10; i++)
        free(player1.grid[i]);
    free(player1.grid);
    for (int i = 0; i < 10; i++)
        free(player2.grid[i]);
    free(player2.grid);

    return 0;
}

char *getShipName(int i)
{
    char *name;
    switch (i)
    {
    case 5:
        name = "carrier";
        break;
    case 4:
        name = "battleship";
        break;
    case 3:
        name = "destroyer";
        break;
    case 2:
        name = "submarine";
        break;
    }
    return name;
}

int **createGrid()
{
    int **grid;
    grid = (int **)malloc(sizeof(int *) * 10);
    if (grid == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    for (int i = 0; i < 10; i++)
    {
        grid[i] = (int *)malloc(sizeof(int) * 10);
        if (grid[i] == NULL)
        {
            printf("Failed to allocate needed memory\n");
            exit(1);
        }
    }
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            grid[i][j] = -1;
        }
    }
    return grid;
}

void displayGrid(Player *player) // tested, displays the grid as desired
{
    printf("%s:\n   A B C D E F G H I J\n", player->name);
    for (int i = 0; i < 10; i++)
    {
        printf("%2d", i + 1);
        for (int j = 0; j < 10; j++)
        {
            char c;
            switch (player->grid[i][j])
            {
            case miss:         // miss = 0
                if (mode == 0) // easy mode, show miss
                    c = 'o';
                else // hard mode, don't show miss
                    c = '~';
                break;
            case hit: // hit = 1
                c = '*';
                break;
            default: // undiscovered: cell empty or has a ship (ships not displayed on grid)
                c = '~';
                break;
            }
            printf(" %c", c);
        }
        printf("\n");
    }
}

void placeShips(Player *player)
{
    printf("These are your ships and their sizes: \ncarrier, 5 cells\nbattleship, 4 cells\ndestroyer, 3 cells\nsubmarine, 2 cells\n");
    printf("Please provide the coordinates and orientation to place each ship on the grid (e.g. B3, horizontal).\n");
    printf("The coordinates indicate the starting position of the ship. Horizontal orientation moves from left to right, and vertical orientation moves from top to bottom.\n");
    printf("Follow this exact format: \ncoordinates: column: CAPITAL_LETTER (A->J), row: number (1->10)\norientation: 'h' for horizontal, 'v' for vertical\n");
    printf("Your input:\n");
    for (int i = 5; i > 1; i--)
    { // acc. to enum, i = 5 corresponds to carrier whose size is 5, and so on, until we reach i = 2 which corresponds to submarine whose size is 2
        char *name = getShipName(i);
        int row;
        char col, orientation;
        printf("(ship name: %s, ship size: %d)\n", name, i);
        printf("column (A->J): ");
        scanf(" %c", &col);
        printf("row (1->10): ");
        scanf("%d", &row);
        printf("orientation (h/v): ");
        scanf_s(" %c", &orientation, 1);
        int hStartingIndex = col - 65; // user enters col as char, here it is used as its ascii value. map value to index 0->9
        int vStartingIndex = row - 1;  // user enter row as int 1->10, map it to int 0->9
        if (hStartingIndex >= 0 && hStartingIndex <= 9 && vStartingIndex >= 0 && vStartingIndex <= 9 && (orientation == 'h' || orientation == 'v'))
        { // followed input format correctly
            if (orientation == 'h' && 10 - hStartingIndex >= i)
            {                  // chosen positon does not extend beyond the grid horizontally
                int count = 0; // count the nb of empty cells
                for (int j = hStartingIndex; j < hStartingIndex + i; j++)
                { // for each cell that the ship would occupy
                    if (player->grid[vStartingIndex][j] == empty)
                    {
                        count++;
                    }
                }
                if (count == i)
                { // space that does not overlap with another ship available
                    for (int j = hStartingIndex; j < hStartingIndex + i; j++)
                    {
                        player->grid[vStartingIndex][j] = i; // position the ship by filling the needed cells of the grid with its corresponding number
                    }
                }
                else
                {
                    printf("Error: chosen coordinates overlap with another ship! Try again:\n");
                    while (getchar() != '\n')
                        ; // Clear input buffer
                    i++;  // i stays the same during the next iteration
                }
            }
            else if (orientation == 'v' && 10 - vStartingIndex >= i)
            {                  // chosen position does not extend beyond the grid vertically
                int count = 0; // count the nb of empty cells
                for (int j = vStartingIndex; j < vStartingIndex + i; j++)
                { // for each cell that the ship would occupy
                    if (player->grid[j][hStartingIndex] == empty)
                    {
                        count++;
                    }
                }
                if (count == i)
                { // space that does not overlap with another ship available
                    for (int j = vStartingIndex; j < vStartingIndex + i; j++)
                    {
                        player->grid[j][hStartingIndex] = i; // position the ship by filling the needed cells of the grid with its corresponding number
                    }
                }
                else
                {
                    printf("Error: chosen coordinates overlap with another ship! Try again:\n");
                    while (getchar() != '\n')
                        ; // Clear input buffer
                    i++;  // i stays the same during the next iteration
                }
            }
            else
            {
                printf("Error: chosen coordinates extend beyond the grid! Try again:\n");
                while (getchar() != '\n')
                    ; // Clear input buffer
                i++;  // i stays the same during the next iteration
            }
        }
        else
        {
            printf("Invalid input format! Please try again while following the specified input format directions:\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            i++;
        }
    }

    getchar(); 
    
    printf("Done placing your ships! Press enter to proceed\n");

    getchar(); // wait for the user to enter a character ('\n') before clearing the screen

    system("cls"); // clear the screen to preserve game secrecy, run using command promt (not visual studio code) to see the clearing
}
