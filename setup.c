#include <stdio.h>

// 10x10 GRID intitialization and display:
char **createGrid()
{
    char **grid;
    grid = (char **)malloc(sizeof(char*) * 10);
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

void displayGrid(char **grid)//shouldnt we display A-J and 1-10
{
    for (int i = 0; i < 10; i++)
    {
        printf("%c", grid[i][0]);
        for (int j = 1; j < 10; j++)
        {
            printf(" %c", grid[i][j]);
        }
        printf("\n");
    }
}

char* mode;
void chooseMode(){
    printf("Choose game mode (easy,hard):\n");
    scanf("%s", &mode);
    for (int i = 0; i < strlen(mode); i++) {
        mode[i] = tolower(mode[i]);
    }
    
}

void modifyingGrid(char** grid, char* mode){
    if(mode == "hard"){
        for(int i = 0; i < 10; i++){
            for(int j = 0; j < 10; j++){
                if(grid[i][j] == 'o'){
                    grid[i][j] == '~';
                }
            }
        }
    }
}

struct player{
    char* name;
    char** grid;
};

void playerName(struct player *player){
    printf("Enter your player name: ");
    scanf("%s", player->name);
}

void playerTurn(const char* playerName1, const char* playerName2){

}

//ship placement
    //function to allow players to place ships on grid one by one(coordinates and orientation)
    //check for valid starting coordinates,no overlapping,no ships beyond grid boundaries
    //clear screen affter each player finishes ship placement to keep positions secret
