#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MOVES_SIZE 5
#define SHIP_SIZE 4
#define GRID_SIZE 10

typedef struct Player Player;
typedef struct Move Move;
typedef struct Ship Ship;
typedef struct Node Node;
typedef struct SmokedNodes SmokedNodes;

// each player has a name and a grid:
struct Player
{
    char name[100]; // reading at most 99 characters for player's name
    int** grid;
    int shipsSunk; // track number of ships that have been sunk
    Move *moves; // each player has a list of moves
    Ship *ships;
    SmokedNodes* smokedNodes //list of smoked coordinates
};

// Structure representing a ship in the game
struct Ship {
    char name[20]; // Name of the ship
    int remainingHits; // Number of remaining hits until the ship sinks
    int symbol; // Symbol which represents the ship in the grid
};

// Structue representing moves 
struct Move {
    char name[50]; // Name of the move
    int counter; // Keeps track of the number of moves
    int shipsSunkToUnlock; // Number of ships sunk to unlock move
};

struct Node {
    char* col;
    int row;
    Node* next;
};

struct SmokedNodes {
    Node* head;
};

/*NOTE!!! for the cells of the grid:
empty: int -1, char '~'
miss: int 0, char 'o'
hit: int 1, char '*'
*/
enum cellStates
{
    empty = -1,
    miss,
    hit,
};


// given an int i (2->5), returns the conrresponding ship's name (only called on i within the specified range)
char *getShipName(int i);

// greate a 10x10 grid, initially empty
int **createGrid();

// display the grid
void displayGrid(Player *player);

// function for placing the ships on the grid
void placeShips(Player *player);

Player createPlayer();

int* getSymbol(Player player, char* col, int* row);

void freeAll(Player player);

// global variable mode, i.e. tracking difficulty level chosen by the user
int mode;

int main()
{
    srand(time(NULL)); // seed the randon number generator with current time, to ensure different output on different runs

    // game setup and initialization:
    // create player1 and player2, read their names, initialize and display their grids
    // read game "mode", i.e. tracking difficulty level (0 for easy, 1 for hard)

    Player player1 = createPlayer();
    Player player2 = createPlayer();

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
    printf("Now it's %s's turn!\n", B->name);
    placeShips(B);

    // CONSIDER: adding more conditions to the code so far to handle invalid inputs from the user

    // next: start game

    // free memory allocated for the players
    freeAll(player1);
    freeAll(player2);

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

int ** createGrid()
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

Ship* createShips() {
    Ship* ships = (Ship *)malloc(sizeof(Ship) * SHIP_SIZE);
    const char *names[SHIP_SIZE] = {"Submarine", "Destroyer", "Battleship", "Carrier"};

    if (ships == NULL) {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }

    for (int i = 0; i < SHIP_SIZE; i++) {
        strcpy(ships[i].name, names[i]);
        ships[i].symbol = i+2;
        ships[i].remainingHits = ships[i].symbol;
    }

    return ships;
}

Move* createMoves() {
    Move* moves = (Move*) malloc(sizeof(Move) * MOVES_SIZE);

    if (moves == NULL) {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }

    const char *names[MOVES_SIZE] = {"Fire", "Radar Sweep", "Smoke Screen", "Artillery", "Torpedo"};
    const int initialCounterVal[MOVES_SIZE] = {-1, 3, 0, 0, 0};
    const int valueToUnlock[MOVES_SIZE] = {0, 0, 1, 1, 3};

    for(int i = 0; i < MOVES_SIZE; i++) {
        strcpy(moves[i].name, names[i]);
        moves[i].counter = initialCounterVal[i];
        moves[i].shipsSunkToUnlock = valueToUnlock[i];
    }

    return moves;

}
void createList(Player player){//if player sinks their first ship we create this list for them
        
    player.smokedNodes = (SmokedNodes*)malloc(sizeof(SmokedNodes));
    if(player.smokedNodes == NULL){
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    
    player.smokedNodes->head = NULL;

}

int inList(Player player, char* col, int row){//to check if the coordinates are in the list
    
    Node* current = player.smokedNodes-> head;
    while(current != NULL){
        if(strcmp(current->col,col) == 0 && current->row==row){
            return 1;
        }
        current = current->next;
    }
    return 0;
}

Node* createSmokedNode(char* col, int row){
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(newNode == NULL){
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    newNode->col = col;
    newNode->row = row;
    newNode->next = NULL;
    return newNode;
}

Player createPlayer() {
    Player player;
    player.grid = createGrid();
    player.ships = createShips();
    player.moves = createMoves();

    return player;
}

void freeAll(Player player) {
    for (int i = 0; i < 10; i++) {
        free(player.grid[i]);
    }

    free(player.grid);
    free(player.ships);
    free(player.moves);
    free(player.smokedNodes);
}

int* getSymbol(Player player, char* col, int* row) {
    
    int colIndex = col - 65;
    int rowIndex = row - 1; 

    return player.grid[colIndex][rowIndex];
}

void displayGrid(Player *player) // tested, displays the grid as desired
{
    printf("%s:\n   A B C D E F G H I J\n", player->name);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        printf("%2d", i + 1);
        for (int j = 0; j < GRID_SIZE; j++)
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
            if (orientation == 'h' && GRID_SIZE - hStartingIndex >= i)
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
            else if (orientation == 'v' && GRID_SIZE - vStartingIndex >= i)
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

void makeMove(Player player, Player opponent, char* col, int* row){
    while(1) {
        printf("Available moves (0-4): \n");
        printf("\t%s : unlimited\n", player.moves[0].name);
        for(int i = 1; i < MOVES_SIZE; i++) {
            printf("\t%s : %d\n", player.moves[i].name, player.moves[i].counter);
        }

        int choice;
        scanf("%d", &choice);

        if (choice < 0 || choice > MOVES_SIZE-1) {
            printf("Please choose a valid value between 0 and 4!\n");
            continue;
        }
        
        if(player.moves[choice].counter == 0){
            printf("Oops! You don't have any %s.\nYou lost your turn :(\n", player.moves[choice].name);
            break;
        }

        player.moves[choice].counter--;

        switch (choice)
        {
        case 0:
            fire(opponent, col, row);
            break;
        case 1:
            radarSweep(player, col, row);
            break;
        case 2:
            smokeScreen(opponent, col, row);
            break;
        case 3:
            artillery(opponent, col, row);
            break;
        case 4:
            torpedo(opponent, col, row);
            break;
        }

        //update game state (if game over, increment any moves etc...)
        break;
        
    }  
}

//fire: target specific coordinates
void fire(Player opponent, char* col, int* row) {
    
    int colIndex = col - 65; 
    int rowIndex = row - 1;

    int* symbol = getSymbol(opponent, col, row);

    if(symbol == hit || symbol == miss || symbol == empty) {
        opponent.grid[rowIndex][colIndex] = miss;
        if(mode == 0){
            printf("miss");
        }
        return;
    }

    for(int i = 0; i < SHIP_SIZE; i++){
        if(symbol == opponent.ships[i].symbol) {
            opponent.ships[i].remainingHits--;
            opponent.grid[rowIndex][colIndex] = hit;
            printf("hit");
            return;
        }
    }
}
//radar sweep:reveal if there are any ships in a 2x2 area
//(only 3 uses)
void radarSweep(Player opponent, char* col, int* row){
    
    int colIndex = col - 65; 
    int rowIndex = row - 1;

    for (int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++) {
            int* gridSymbol = opponent.grid[rowIndex + i][colIndex + j];
            if(gridSymbol != hit && gridSymbol != miss && gridSymbol != empty){
                if(inList(opponent, col, row) == 0){
                    printf("Enemy ships found!");
                    return;
                }
            }
        }
    }
    
    printf("No enemy ships found!");

}
//smoke screen: obscure a 2x2 area
//(reward only after sinking a ship)
void smokeScreen(Player player, char* col, int* row){
    
    int colIndex = col - 65; 
    int rowIndex = row - 1;

    for (int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++) {
            int* gridSymbol = player.grid[rowIndex + i][colIndex + j];
            if(gridSymbol != hit && gridSymbol != miss && gridSymbol != empty) {
                if(inList(player, col, row) == 0){
                    Node* newNode = createSmokedNode(col,row);
                    newNode->next = player.smokedNodes->head;
                    player.smokedNodes->head = newNode;
                }
            }
        }
    }
}

//artillery: 2x2 attack area
//(unlocked after sinking a ship)
void artillery(Player opponent, char* col, int* row){

    int colIndex = col - 65; 
    int rowIndex = row - 1;

    for (int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++) {
            int* symbol = opponent.grid[rowIndex + i][colIndex + j];
            
            if(symbol == hit || symbol == miss || symbol == empty) {
                opponent.grid[rowIndex][colIndex] = miss;
                if(mode == 0){
                    printf("miss");
                }
                continue;
            }

            for(int y = 0; y < SHIP_SIZE; y++){
                if(symbol == opponent.ships[i].symbol) {
                    opponent.ships[i].remainingHits--;
                    opponent.grid[rowIndex][colIndex] = hit;
                    printf("hit");
                }
            }
        }
    }

}

//torpedo: a row or column attack
//(unlocked after sinking the third ship)
void torpedo(Player opponent, char* col, int* row){
    
    int colIndex = col - 65; 
    int rowIndex = row - 1;

    //if player chose row
    if(col == NULL){
        for(int j = 0; j < GRID_SIZE; j++){
        int* symbol = opponent.grid[rowIndex][colIndex + j];
        
        if(symbol == hit || symbol == miss || symbol == empty) {
                opponent.grid[rowIndex][colIndex] = miss;
                if(mode == 0){
                    printf("miss");
                }
                continue;
        }

        if(symbol == opponent.ships[j].symbol) {
            opponent.ships[j].remainingHits--;
            opponent.grid[rowIndex][colIndex] = hit;
            printf("hit");
        }
    }
    }

    //if player chose col
    if(row == NULL){
        for(int i = 0; i < GRID_SIZE; i++){
        int* symbol = opponent.grid[rowIndex + i][colIndex];

        if(symbol == hit || symbol == miss || symbol == empty) {
                opponent.grid[rowIndex][colIndex] = miss;
                if(mode == 0){
                    printf("miss");
                }
                continue;
        }

        if(symbol == opponent.ships[i].symbol) {
            opponent.ships[i].remainingHits--;
            opponent.grid[rowIndex][colIndex] = hit;
            printf("hit");
        }
    }
    }

}
