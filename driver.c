/*run command prompt for optimal "clear screen" results*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MOVES_COUNT 5
#define SHIPS_COUT 4
#define GRID_SIZE 10

// player: name, grid, ships, moves:
typedef struct player
{
    char name[100];
    int **grid;
    Ship *ships;
    Move *moves;
    int shipsSunkCount;
    SmokedCells *smokedCells;

} Player;

typedef struct ship {
    char name[20];
    int remainingHits; // nb of hits remaining until the ship sinks
} Ship;

typedef struct move {
    char name[50];
    int coutnAvailable;
    int shipsSunkToUnlock; 
} Move;

typedef struct cell {
    char* col;
    int row;
    Cell* next;
} Cell;

typedef struct smokedCells {
    smokedCells* head;
} SmokedCells;

// for the cells of the grid:
enum cellStates
{
    empty = -1, // '~'
    miss = 0,   // 'o'
    hit = 1,    // '*'
    submarine = 2,
    destroyer = 3,
    battleship = 4,
    carrier = 5
};

// FUNCTIONS:

Player createPlayer();
Ship* createShips();
Move* createMoves();
char *getShipName(int i); // i: 2->5

// grid:
int **createGrid();
void displayGrid(Player *player);

// ship placement:
void placeShips(Player *player);
void placeShip(Player *player, int shipSize);
int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation);

// game play:
void takeTurn(Player *player, Player *opponent);
void displayAvailableMoves(Player *player);
void makeMove(Player *player, Player* opponent);
int gameOver(Player *player, Player *opponent);

// moves:

void freeAll(Player player);

// tracking difficulty level
int mode;

int main()
{

    /*-------------------------------------------------Game Setup and Initialization-------------------------------------------------------*/

    srand(time(NULL)); // seed the random number generator with current time

    Player player1 = createPlayer();
    Player player2 = createPlayer();

    // read game mode:
    printf("Welcome to MAN.BattleShip! Please choose the tracking difficulty level by entering '0' for easy and '1' for hard: ");
    scanf_s("%d", &mode);

    printf("\n");

    // read players names:
    printf("Please enter your names!\nPlayer1: ");
    scanf_s("%99s", player1.name, 100);
    printf("Player2: ");
    scanf_s("%99s", player2.name, 100);

    printf("\n");

    // display grids:
    printf("%s: \n", player1.name);
    displayGrid(&player1);
    printf("\n");
    printf("%s: \n", player2.name);
    displayGrid(&player2);

    printf("\n");

    // randomly choose starting player:
    int i = rand() % 2;
    Player *startingPlayer;
    Player *otherPlayer;
    if (i == 0)
    {
        startingPlayer = &player1;
        otherPlayer = &player2;
    }
    else
    {
        startingPlayer = &player2;
        otherPlayer = &player1;
    }

    // place ships:
    printf("Placing your ships: \n");
    printf("\n");
    getchar();
    printf("%s will start!\n", startingPlayer->name);
    printf("\n");
    getchar();
    system("cls");
    placeShips(startingPlayer);
    printf("Now is %s's turn!\n", otherPlayer->name);
    printf("\n");
    getchar();
    system("cls");
    placeShips(otherPlayer);

    /*-----------------------------------------------------------Game Play-----------------------------------------------------------------*/

    printf("Now we will proceed with the game :)\n");
    printf("\n");

    // game loop:
    while (1)
    {
        takeTurn(startingPlayer, otherPlayer);
        if (gameOver(startingPlayer, otherPlayer))
            break;
        takeTurn(otherPlayer, startingPlayer);
        if (gameOver(otherPlayer, startingPlayer))
            break;
    }

    // free memory allocated for the players:
    freeAll(player1);
    freeAll(player2);

    return 0;
}

/*-------------------------------------------------Game Setup and Initialization-------------------------------------------------------*/

Player createPlayer()
{
    Player p;
    p.grid = createGrid();
    return p;
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
    grid = (int **)malloc(sizeof(int *) * GRID_SIZE);
    if (grid == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    for (int i = 0; i < GRID_SIZE; i++)
    {
        grid[i] = (int *)malloc(sizeof(int) * GRID_SIZE);
        if (grid[i] == NULL)
        {
            printf("Failed to allocate needed memory\n");
            // free what have already been allocated:
            for (int j = 0; j < i; j++)
                free(grid[j]);
            free(grid);
            exit(1);
        }
    }
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = -1;
        }
    }
    return grid;
}

void displayGrid(Player *player)
{
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < GRID_SIZE; i++)
    {
        printf("%2d", i + 1);
        for (int j = 0; j < GRID_SIZE; j++)
        {
            char c;
            switch (player->grid[i][j])
            {
            case miss:
                if (mode == 0) // easy mode, show miss
                    c = 'o';
                else // hard mode, don't show miss
                    c = '~';
                break;
            case hit:
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
    // instructions:
    printf("These are your ships and their sizes: \ncarrier, 5 cells\nbattleship, 4 cells\ndestroyer, 3 cells\nsubmarine, 2 cells\n");
    printf("\n");
    printf("Please provide the coordinates and orientation to place each ship on the grid (e.g. B3, Horizontal).\n");
    printf("The coordinates indicate the starting position of the ship. Horizontal orientation moves from left to right, and vertical orientation moves from top to bottom.\n");
    printf("Follow this exact format: \ncoordinates: column: CAPITAL_LETTER (A->J), row: number (1->10)\norientation: 'H' for horizontal, 'V' for vertical\n");
    printf("\n");
    printf("Your input:\n");

    for (int i = 5; i > 1; i--) // i = ship size 
    {
        placeShip(player, i);
    }

    getchar();

    printf("Done placing your ships! Press enter to proceed\n");

    getchar();
    system("cls");
}

void placeShip(Player *player, int shipSize) 
{
    char *name = getShipName(shipSize);
    int rw;
    char cl, orientation;

    // read position:
    printf("\n(ship name: %s, ship size: %d)\n", name, shipSize);
    printf("column (A->J): ");
    scanf(" %c", &cl);
    printf("row (1->10): ");
    scanf("%d", &rw);
    printf("orientation (H/V): ");
    scanf_s(" %c", &orientation, 1);
    printf("\n");

    // map input to coordinates on the grid with 0-based index:
    int col = cl - 65;
    int row = rw - 1;

    // validate coordinates, and place ship or try again:
    if (canPlaceShip(player, shipSize, row, col, orientation)) // place this ship, then move on to the next
    {
        if (orientation == 'H')
        {
            for (int j = col; j < col + shipSize; j++)
            {
                player->grid[row][j] = shipSize;
            }
        }
        else
        {
            for (int j = row; j < row + shipSize; j++)
            {
                player->grid[j][col] = shipSize;
            }
        }
    }
    else // try agin with same ship
    {
        placeship(player, shipSize);
    }
}

int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation)
{
    if (col < 0 || col > 9 || row < 0 || row > 9 || (orientation != 'H' && orientation != 'V'))
    {
        printf("Invalid input format! Please try again while following the specified input format directions:\n");
        return 0;
    }
    else
    {
        if (orientation == 'H')
        {
            if (col + shipSize > 10)
            {
                printf("Error: chosen coordinates extend beyond the grid! Try again:\n");
                return 0;
            }
            for (int j = col; j < col + shipSize; j++)
            {
                if (player->grid[row][j] != empty)
                {
                    printf("Error: chosen coordinates overlap with another ship! Try again:\n");
                    return 0;
                }
            }
        }
        else if (orientation == 'V')
        {
            if (row + shipSize > 10)
            {
                printf("Error: chosen coordinates extend beyond the grid! Try again:\n");
                return 0;
            }
            for (int j = row; j < row + shipSize; j++)
            {
                if (player->grid[j][col] != empty)
                {
                    printf("Error: chosen coordinates overlap with another ship! Try again:\n");
                    return 0;
                }
            }
        }
    }

    return 1;
}

/*-----------------------------------------------------------Game Play-----------------------------------------------------------------*/

void takeTurn(Player *player, Player *opponent)
{
    printf("%s's Turn! Press enter to proceed \n", player->name);
    getchar();
    system("cls");

    printf("%s's current grid: \n\n", opponent->name);
    displayGrid(opponent);

    printf("Available moves: \n\n");
    displayAvailableMoves(player);

    makeMove(Player *player, Player* opponent);

    /*make move (fire, radar sweep, smoke screen, artillery, torpedo):
     take coordinates, if invalid player looses their turn
     make necessary changes to the grid 
     */
    // clear screen
    // announce result of the move (hit/miss, etc.), and if a ship has been sunk

    printf("%s's updated grid: \n\n", opponent->name);
    displayGrid(opponent);

    printf("\nPress enter to proceed!\n");
    getchar();
    system("cls");
}

void displayAvailableMoves(Player *player)
{
    // conditions to unlock certain moves, satisfied?
    // fire 'F'
    // radarSweep 'R'
    // smokeScreen 'S'
    // artillery 'A'
    // torpedo 'T'
    printf("- enter '0' for FIRE, targets a cell of the opponent's grid\n");
    print("")
    printf();

}

void makeMove(Player *player, Player *opponenet) 
{
    char move;
    printf("Enter your chosen move: ");
    scanf(" %c", &move);

    switch(move) 
    {
        case 'F':
        // ...
        break;

        case 'R':
        // ...
        break;

        case 'S':
        // ...
        break;

        case 'A':
        // ...
        break;

        case 'T':
        // ...
        break;

        default:
        printf("Invalid input! Please chose again from the list of available moves using the specified format\n\n");
            playerTurn(player, opponent);
            break;
    }
}

int gameOver(Player *player, Player *opponent)
{
    if (opponent->shipsSunkCount == SHIPS_COUNT)
    {
        printf("All of %s's ships have been sunk! %s wins, congrats :)", opponent->name, player->name);
        getchar();
        return 1;
    }
    else
    {
        return 0;
    }
}

void freeAll(Player player)
{
    for (int i = 0; i < 10; i++)
    {
        free(player.grid[i]);
    }
    free(player.grid);
}

