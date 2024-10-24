#include <stdlib.h>
#include <stdio.h>

// each player has a name and a grid: 
typedef struct palyer Player;
struct palyer
{
    char *name;
    char **grid;
};

// greate a 10x10 grid, initially filled with '~'
// display the grid
char **createGrid();
void displayGrid(Player *player);

int main()
{
    // game setup and initialization: 
    // create 2 player pointers: player1 and player2, allocate the needed memory
    // read player names, allocate memory for names and grids and display them
    // read game "mode", i.e. tracking difficulty level (0 for easy, 1 for hard)
    Player *player1 = (Player *)malloc(sizeof(Player));
    if (player1 == NULL)
    {
        printf("Failed to allocate memory for player1\n");
        return 1;
    }
    Player *player2 = (Player *)malloc(sizeof(Player));
    if (player2 == NULL)
    {
        printf("Failed to allocate memory for player2\n");
        free(player1);
        return 1;
    }
    player1->grid = createGrid();
    player2->grid = createGrid();
    printf("Welcome to MAN.BattleShip! Please choose the tracking difficulty level by entering '0' for easy and '1' for hard: ");
    int mode;
    scanf_s("%d", &mode);
    player1->name = (char *)malloc(100 * sizeof(char)); // reading 99 characters at most for player names
    if (player1->name == NULL)
    {
        printf("Failed to allocate memory for player1's name\n");
        free(player1);
        free(player2);
        return 1;
    }
    player2->name = (char *)malloc(100 * sizeof(char));
    if (player2->name == NULL)
    {
        printf("Failed to allocate memory for player2's name\n");
        free(player1->name);
        free(player1);
        free(player2);
        return 1;
    }
    printf("Please enter your names!\nPlayer1: ");
    scanf_s("%99s", player1->name, 100);
    printf("Player2: ");
    scanf_s("%99s", player2->name, 100);
    displayGrid(player1);
    displayGrid(player2);

    // proceed with: 
    // randomly choose the starting player
    // note that when the game starts, maintain alternating turns
    // players in turn position their ships
    // note: clear the screen in between turns
    // consider: the conditions for placing, how to show these updates on the grid, how to track them, etc.

    // Free allocated memory to avoid memory leaks
    free(player1->name);
    for (int i = 0; i < 10; i++)
        free(player1->grid[i]);
    free(player1->grid);
    free(player1);
    free(player2->name);
    for (int i = 0; i < 10; i++)
        free(player2->grid[i]);
    free(player2->grid);
    free(player2);

    return 0;
}

char **createGrid()
{
    char **grid;
    grid = (char **)malloc(sizeof(char *) * 10);
    for (int i = 0; i < 10; i++)
    {
        grid[i] = (char *)malloc(sizeof(char) * 10);
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
