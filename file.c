#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// player: name, grid:
typedef struct player
{
    char name[100];
    int **grid;
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
    carrier = 5
};

// FUNCTIONS:

Player createPlayer();
char *getShipName(int i); // i: 2->5

// grid:
int **createGrid();
void displayGrid(Player *player);

// ship placement:
int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation);
void placeShips(Player *player);

// moves:

void freeAll(Player player);

// tracking difficulty level
int mode;

int main()
{

    /*-----------------------------------------------Game Setup and Initialization--------------------------------------------------*/

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
    displayGrid(&player1);
    printf("\n");
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

    // free memory allocated for the players
    freeAll(player1);
    freeAll(player2);

    return 0;
}

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
            // free what have already been allocated:
            for (int j = 0; j < i; j++)
                free(grid[j]);
            free(grid);
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

void displayGrid(Player *player)
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
    printf("Please provide the coordinates and orientation to place each ship on the grid (e.g. B3, horizontal).\n");
    printf("The coordinates indicate the starting position of the ship. Horizontal orientation moves from left to right, and vertical orientation moves from top to bottom.\n");
    printf("Follow this exact format: \ncoordinates: column: CAPITAL_LETTER (A->J), row: number (1->10)\norientation: 'h' for horizontal, 'v' for vertical\n");
    printf("\n");
    printf("Your input:\n");

    for (int i = 5; i > 1; i--)
    {
        char *name = getShipName(i);
        int rw;
        char cl, orientation;

        // read position:
        printf("\n(ship name: %s, ship size: %d)\n", name, i);
        printf("column (A->J): ");
        scanf(" %c", &cl);
        printf("row (1->10): ");
        scanf("%d", &rw);
        printf("orientation (h/v): ");
        scanf_s(" %c", &orientation, 1);
        printf("\n");

        // map input to coordinates on the grid with 0-based index:
        int col = cl - 65;
        int row = rw - 1;

        // validate coordinates, and place ship or try again:
        if (canPlaceShip(player, i, row, col, orientation)) // place this ship, then move on to the next
        {
            if (orientation == 'h')
            {
                for (int j = col; j < col + i; j++)
                {
                    player->grid[row][j] = i;
                }
            }
            else
            {
                for (int j = row; j < row + i; j++)
                {
                    player->grid[j][col] = i;
                }
            }
        }
        else // try agin with same ship
        {
            i++;
        }
    }

    getchar();

    printf("Done placing your ships! Press enter to proceed\n");

    getchar();
    system("cls");
}

int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation)
{
    if (col < 0 || col > 9 || row < 0 || row > 9 || (orientation != 'h' && orientation != 'v'))
    {
        printf("Invalid input format! Please try again while following the specified input format directions:\n");
        return 0;
    }
    else
    {
        if (orientation == 'h')
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
        else if (orientation == 'v')
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

void freeAll(Player player)
{
    for (int i = 0; i < 10; i++)
    {
        free(player.grid[i]);
    }
    free(player.grid);
}
