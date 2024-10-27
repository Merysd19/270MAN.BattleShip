/*run command prompt for optimal "clear screen" results*/ //TEST 

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MOVES_COUNT 5
#define SHIPS_COUNT 4
#define GRID_SIZE 10

typedef struct ship
{
    char name[20];
    int remainingHits; // number of remaining hits until the ship sinks
} Ship;

typedef struct move
{
    char name[50];
    int countAvailable;
} Move;

struct cell 
{
    int col;
    int row;
    cell *next;
};

typedef struct cell Cell;

typedef struct smokedCells
{
    Cell *head;
} SmokedCells;

// player:
typedef struct player
{
    char name[100];
    int **grid;
    int shipsCount;
    Ship *ships;
    Move *moves;
    SmokedCells *smokedCells;

} Player;


// for the cells of the grid:
enum cellStates
{
    empty = -1, // '~'
    miss = 0,   // 'o'
    hit = 1,    // '*'
    submarine = 2,
    destroyer = 3,
    battleship = 4,
    carrier = 5,
};

// FUNCTIONS:

Player createPlayer();
Ship *createShips();
Move *createMoves();
SmokedCells *createSmokedList();

// grid:
int **createGrid();
void displayGrid(Player *player);

// ship placement:
void placeShips(Player *player);
void placeShip(Player *player, int shipSize);
int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation);
char *getShipName(int i); // i: 2->5

// game play:
void takeTurn(Player *player, Player *opponent);
void displayAvailableMoves(Player *player, Player *opponent);
int makeMove(Player *player, Player *opponent);
int gameOver(Player *player, Player *opponent);

// moves + their helper functions:
int fire(Player *player, Player *opponent);
int radarSweep(Player *player, Player *opponent);
int smokeScreen(Player *player, Player *opponent);
int artillery(Player *player, Player *opponent);
int torpedo(Player *player, Player *opponent);
int checkAvailable(Player *player, int move);
int validTopLeftCoordinate(int row, int col);
Cell *createSmokedCell(int col, int row);
int inSmokedList(Player *opponent, int row, int col);

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

    printf("Now we will proceed with the game :) \n");
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
    Player player;
    player.grid = createGrid();
    player.shipsCount = 4;
    player.ships = createShips();
    player.moves = createMoves();
    player.smokedCells = createSmokedList();
    return player;
}

Ship *createShips()
{
    Ship *ships = (Ship *)malloc(sizeof(Ship) * SHIPS_COUNT);
    if (ships == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    for (int i = 2; i <= 5; i++)
    {
        strcpy(ships[i - 2].name, getShipName(i));
        ships[i - 2].remainingHits = i;
    }
    return ships;
}

Move *createMoves()
{
    Move *moves = (Move *)malloc(sizeof(Move) * MOVES_COUNT);
    if (moves == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    const char *names[MOVES_COUNT] = {"FIRE", "RADAR SWEEP", "SMOKE SCREEN", "ARTILLERY", "TORPEDO"};
    int counts[MOVES_COUNT] = {-1, 3, 0, 0, 0};
    for (int i = 0; i < MOVES_COUNT; i++)
    {
        strcpy(moves[i].name, names[i]);
        moves[i].countAvailable = counts[i];
    }
    return moves;
}

SmokedCells *screateSmokedList()
{
    SmokedCells *smokedCells = (SmokedCells *)malloc(sizeof(SmokedCells));
    if (smokedCells == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    smokedCells->head = NULL;
    return &smokedCells->head;
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
        placeShip(player, shipSize);
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

/*-----------------------------------------------------------Game Play-----------------------------------------------------------------*/

void takeTurn(Player *player, Player *opponent)
{
    printf("%s's Turn! Press enter to proceed \n", player->name);
    getchar();
    system("cls");

    printf("%s's current grid: \n\n", opponent->name);
    displayGrid(opponent);

    printf("Available moves: \n\n");
    displayAvailableMoves(player, opponent);

    if (makeMove(player, opponent)) // move complete, player did not loose their turn
    {
        /*make move (fire, radar sweep, smoke screen, artillery, torpedo):
     take coordinates, if invalid player looses their turn
     make necessary changes to the grid
     */
        // clear screen
        // announce result of the move (hit/miss, etc.), and if a ship has been sunk

        printf("%s's updated grid: \n\n", opponent->name);
        displayGrid(opponent);
    }

    printf("\nPress enter to proceed!\n");
    getchar();
    system("cls");
}

void displayAvailableMoves(Player *player, Player *opponent)
{
    printf("FIRE, targets a cell of the opponent's grid.\n");
    printf("Input format: identifier: '0', coordinate (e.g. B3)\n\n");
    for (int i = 1; i < MOVES_COUNT; i++)
    {
        if (player->moves[i].countAvailable > 0)
        {
            if (i == 1)
            {
                printf("RADAR SWEEP, reveals whether there are any opponent ships in a specified 2x2 area of their grid without showing exact locations of ships.\n");
                printf("Input format: identifier: '1', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n");
            }
            if (i == 2)
            {
                printf("SMOKE SCREEN, obscures a 2x2 area of your grid by hiding it from radar sweeps.\n");
                printf("Input format: identifier: '2', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n");
            }
            if (i == 3)
            {
                printf("ARTILLERY, an attack move that works similarly to FIRE but targets a 2x2 area.\n");
                printf("Input format: identifier: '3', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n");
            }
            if (i == 4)
            {
                printf("TORPEDO, powerful attack that targets an entire row or column.\n");
                printf("Input format: identifier: '4', either row number (1->10) or column letter (A->J).\n");
            }
        }
    }
    printf("\n");
}

int makeMove(Player *player, Player *opponent)
{
    int move;
    printf("Enter the identifier for your chosen move: ");
    scanf(" %d", &move);

    switch (move)
    {
    case 0:
        return fire(player, opponent);

    case 1:
        return radarSweep(player, opponent);

    case 2:
        return smokeScreen(player, opponent);

    case 3:
        return artillery(player, opponent);

    case 4:
        return torpedo(player, opponent);

    default:
        printf("Invalid input! Please chose again from the list of available moves using the specified format\n\n");
        makeMove(player, opponent);
        break;
    }
}

int fire(Player *player, Player *opponent)
{
    int rw;
    char cl;

    printf("\nEnter coordinate (e.g. B3): ");
    scanf(" %c%d", &cl, &rw);

    int row = rw - 1;
    int col = cl - 'A';

    if (row < 0 || row > 9 || col < 0 || col > 9)
    {
        printf("Invalid coordinates! You loose your turn :(\n");
        return 0;
    }
    else
    {
        int i = opponent->grid[row][col];
        if (i > 1)
        {
            opponent->grid[row][col] = hit;
            printf("\n\nRresult: hit!\n\n");

            for (int j = 0; j < SHIPS_COUNT; i++)
            {
                if (opponent->ships[j].name == getShipName(i))
                {
                    opponent->ships[j].remainingHits--;
                    if (opponent->ships[j].remainingHits-- == 0)
                    {
                        opponent->shipsCount--;
                        player->moves[2].countAvailable++;
                        player->moves[3].countAvailable = 1;
                        printf("One of %s's ships, a %s, has been sunk!", opponent->name, getShipName(i));
                    }
                }
            }
        }
        else
        {
            opponent->grid[row][col] = miss;
            printf("\n\nRresult: miss!\n\n");
        }
    }
}

int radarSweep(Player *player, Player *opponent)
{
    if (checkAvailable(player, 1))
    {
        int rw;
        char cl;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        int row = rw - 1;
        int col = cl - 'A';

        if (validTopLeftCoordinate(row, col))
        {
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    int gridSymbol = opponent->grid[row + i][col + j];
                    if (gridSymbol != hit && gridSymbol != miss && gridSymbol != empty)
                    {
                        if (inSmokedList(opponent, col, row) == 0)
                        {
                            printf("\n\nResult: enemy ships found!");
                            player->moves[1].countAvailable--;
                            return 1;
                        }
                    }
                }
            }
            printf("\n\nResult: no enemy ships found!");
            player->moves[1].countAvailable--;
            return 1;
        }
    }
    return 0;
}

int smokeScreen(Player *player, Player *opponent)
{
    if (checkAvailable(player, 2))
    {
        int rw;
        char cl;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        int row = rw - 1;
        int col = cl - 'A';

        if (validTopLeftCoordinate(row, col))
        {
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    int gridSymbol = player->grid[row + i][col + j];
                    if (gridSymbol != hit && gridSymbol != miss && gridSymbol != empty)
                    {
                        if (inSmokedList(opponent, col, row) == 0)
                        {
                            Cell *newCell = createSmokedCell(col, row);
                            newCell->next = player->smokedCells->head;
                            player->smokedCells->head = newCell;
                        }
                    }
                }
            }
        }
        player->moves[1].countAvailable--;
        printf("\n\nSMOKE SCREEN performed! Press enter to proceed \n");
        getchar();
        system("cls");
        return 1;
    }
    return 0;
}

int artillery(Player *player, Player *opponent)
{
    if (checkAvailable(player, 3))
    {
        int rw;
        char cl;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        int row = rw - 1;
        int col = cl - 'A';

        if (validTopLeftCoordinate(row, col))
        {
            int r = 0;               //  nb of hits
            int s[4] = {0, 0, 0, 0}; // to track the names of ships that have been sunk
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    int gridSymbol = opponent->grid[row + i][col + j];
                    if (gridSymbol > 1)
                    {
                        opponent->grid[row][col] = hit;
                        r++;
                        for (int k = 0; k < SHIPS_COUNT; i++)
                        {
                            if (opponent->ships[k].name == getShipName(gridSymbol))
                            {
                                opponent->ships[k].remainingHits--;
                                if (opponent->ships[k].remainingHits-- == 0)
                                {
                                    opponent->shipsCount--;
                                    player->moves[2].countAvailable++;
                                    s[k] = gridSymbol;
                                }
                            }
                        }
                    }
                    else
                    {
                        opponent->grid[row][col] = miss;
                    }
                }
            }
            if (r == 0)
            {
                printf("\n\nResult: miss!\n\n");
                player->moves[3].countAvailable = 0;
            }
            else
            {
                printf("\n\nResult: hit!\n\n");
                int nbSunk = 0;
                for (int i = 0; i < SHIPS_COUNT; i++)
                {
                    if (s[i] == i + 2)
                    {
                        printf("One of %s's ships, a %s, has been sunk!", opponent->name, getShipName(i + 2));
                        player->moves[3].countAvailable = 1;
                        if (opponent->shipsCount == 1)
                            player->moves[4].countAvailable = 1;
                        nbSunk++;
                    }
                }
                if (nbSunk == 0)
                    player->moves[3].countAvailable = 0;
            }
        }
        return 1;
    }
    return 0;
}

int torpedo(Player *player, Player *opponent)
{
    if (checkAvailable(player, 4))
    {
        // read target row or column
        char input;
        int row = -1;
        int col = -1;
        printf("\nEnter row (e.g. 3) or column (e.g. B): ");
        scanf(" %c", &input);

        // validate input
        if (input >= 'A' && input <= 'J')
        {
            col = input - 'A';
        }
        else if (input >= '1' && input <= '9' + 1)
        {
            row = input - '1';
        }
        else
        {
            printf("Invalid coordinates! You loose your turn :(\n");
            return 0;
        }

        int r = 0;               //  nb of hits
        int s[4] = {0, 0, 0, 0}; // to track the names of ships that have been sunk

        if (col == -1) // target a row
        {
            for (int i = 0; i < GRID_SIZE; i++)
            {
                int gridSymbol = opponent->grid[row][i];
                if (gridSymbol > 1)
                {
                    opponent->grid[row][i] = hit;
                    r++;
                    for (int k = 0; k < SHIPS_COUNT; i++)
                    {
                        if (opponent->ships[k].name == getShipName(gridSymbol))
                        {
                            opponent->ships[k].remainingHits--;
                            if (opponent->ships[k].remainingHits-- == 0)
                            {
                                opponent->shipsCount--;
                                player->moves[2].countAvailable++;
                                s[k] = gridSymbol;
                            }
                        }
                    }
                }
                else
                {
                    opponent->grid[row][i] = miss;
                }
            }
            if (r == 0)
            {
                printf("\n\nResult: miss!\n\n");
                player->moves[3].countAvailable = 0;
                player->moves[4].countAvailable = 0;
            }
            else
            {
                printf("\n\nResult: hit!\n\n");
                int nbSunk = 0;
                for (int i = 0; i < SHIPS_COUNT; i++)
                {
                    if (s[i] == i + 2)
                    {
                        printf("One of %s's ships, a %s, has been sunk!", opponent->name, getShipName(i + 2));
                        player->moves[3].countAvailable = 1;
                        if (opponent->shipsCount == 1)
                            player->moves[4].countAvailable = 1;
                        nbSunk++;
                    }
                }
                if (nbSunk == 0)
                    player->moves[3].countAvailable = 0;
                player->moves[4].countAvailable = 0;
            }
            return 1;
        }
        else // target a column
        {
            for (int i = 0; i < GRID_SIZE; i++)
            {
                int gridSymbol = opponent->grid[i][col];
                if (gridSymbol > 1)
                {
                    opponent->grid[i][col] = hit;
                    r++;
                    for (int k = 0; k < SHIPS_COUNT; i++)
                    {
                        if (opponent->ships[k].name == getShipName(gridSymbol))
                        {
                            opponent->ships[k].remainingHits--;
                            if (opponent->ships[k].remainingHits-- == 0)
                            {
                                opponent->shipsCount--;
                                player->moves[2].countAvailable++;
                                s[k] = gridSymbol;
                            }
                        }
                    }
                }
                else
                {
                    opponent->grid[i][col] = miss;
                }
            }
            if (r == 0)
            {
                printf("\n\nResult: miss!\n\n");
                player->moves[3].countAvailable = 0;
                player->moves[4].countAvailable = 0;
            }
            else
            {
                printf("\n\nResult: hit!\n\n");
                int nbSunk = 0;
                for (int i = 0; i < SHIPS_COUNT; i++)
                {
                    if (s[i] == i + 2)
                    {
                        printf("One of %s's ships, a %s, has been sunk!", opponent->name, getShipName(i + 2));
                        player->moves[3].countAvailable = 1;
                        if (opponent->shipsCount == 1)
                            player->moves[4].countAvailable = 1;
                        nbSunk++;
                    }
                }
                if (nbSunk == 0)
                    player->moves[3].countAvailable = 0;
                player->moves[4].countAvailable = 0;
            }
            return 1;
        }
    }
    return 0;
}

int checkAvailable(Player *player, int move)
{
    if (player->moves[move].countAvailable == 0)
    {
        printf("\nOops, you don't have an available %s move! You lost your turn :(\n", player->moves[move].name);
        return 0;
    }
    return 1;
}

int validTopLeftCoordinate(int row, int col)
{
    if (row < 9 && row >= 0 && col < 9 && col >= 0)
    {
        return 1;
    }
    printf("Invalid coordinates! You loose your turn :(\n");
    return 0;
}

Cell *createSmokedCell(int col, int row)
{
    Cell *newCell = (Cell *)malloc(sizeof(Cell));
    if (newCell == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    newCell->col = col;
    newCell->row = row;
    newCell->next = NULL;
    return newCell;
}

int inSmokedList(Player *player, int row, int col)
{
    Cell *current = player->smokedCells->head;
    while (current != NULL)
    {
        if (current->col == col && current->row == row)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int gameOver(Player *player, Player *opponent)
{
    if (opponent->shipsCount == 0)
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
    free(player.ships);
    free(player.moves);
}
