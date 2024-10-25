#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// each player has a name and a grid:
typedef struct player Player;
struct player
{
    char name[100]; // reading at most 99 characters for player's name
    char **grid;
    //heyyyyyyyyyyyyyyyyyyyyyyyyyyy
};

// greate a 10x10 grid, initially filled with '~'
// display the grid
char **createGrid();
void displayGrid(Player *player);

// function for placing the ships on the grid
void placeShips(Player *player);

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
    int mode;
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
    // clear the screen to preserve game secrecy, function: system("cls");
    printf("Now is %s's turn!\n", B->name);
    placeShips(B);
    // clear the screen to preserve game secrecy

    // CONSIDER: adding more conditions to the code so far to handle invalid inputs from the user

    // next: placeShips, then: start game

    // free memory allocated for the grids
    for (int i = 0; i < 10; i++)
        free(player1.grid[i]);
    free(player1.grid);
    for (int i = 0; i < 10; i++)
        free(player2.grid[i]);
    free(player2.grid);

    return 0;
}

char **createGrid()
{
    char **grid;
    grid = (char **)malloc(sizeof(char *) * 10);
    if (grid == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    for (int i = 0; i < 10; i++)
    {
        grid[i] = (char *)malloc(sizeof(char) * 10);
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
            grid[i][j] = '~';
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
            printf(" %c", player->grid[i][j]);
        }
        printf("\n");
    }
}

/*
FLEET:
carrier, 5 cells
battleship, 4 cells
destroyer, 3 cells
submarine, 2 cells
*/
// place these ships on the grid, according to the CONDITIONS. > how to reflect these chages on the grid?
// when play starts, how to show hits and misses differently according to mode
// how to track if a ship was sunk
// ->need a another function, moveShips?

// Function to place a ship on the grid
int placeShip(Player *player, int shipSize, const char *shipName) {
    char input[10];
    char orientation;
    int row, col;

    // User input 
    printf("Enter coordinates for %s (e.g., B3 H/V): ", shipName);
    scanf("%s %c", input, &orientation);

    // Convert input to row and column
    col = input[0] - 'A'; // Convert letter to column index (A=0,B=1...)
    row = atoi(input + 1) - 1; //(1=0,2=1.. to match code index )

    // Validate input
    if (col < 0 || col >= 10 || row < 0 || row >= 10) {
        printf("Invalid coordinates :(\n");
        return -1;
    }

    // Validate ship placement 
    if (orientation == 'H') { //horizental checking 
        if (col + shipSize > 10) { //col here is the starting col:  (col+shipSize)=ending col
            printf("Ship does not fit horizontally :(\n");
            return -1;
        }
        // Check for overlapping
        for (int i = 0; i < shipSize; i++) {
            if (player->grid[row][col + i] != '~') { //overlap: a cell is not ~ 
                printf("Ship overlaps with another ship :(\n");
                return -1;
            }
        }
        // Place the ship
        for (int i = 0; i < shipSize; i++) {
            player->grid[row][col + i] = 'S'; // Mark the ship on the grid
        }
    } else if (orientation == 'V') { //vertical checking 
        if (row + shipSize > 10) {
            printf("Ship does not fit vertically :(\n");
            return -1;
        }
        // Check for overlapping
        for (int i = 0; i < shipSize; i++) {
            if (player->grid[row + i][col] != '~') {
                printf("Ship overlaps with another :(\n");
                return -1;
            }
        }
        // Place the ship
        for (int i = 0; i < shipSize; i++) {
            player->grid[row + i][col] = 'S'; // Mark the ship on the grid
        }
    } else {
        printf("Invalid orientation. Use 'H' for horizontal or 'V' for vertical.\n");
        return -1;
    }

    return 0; // Ship is now placed 
}

