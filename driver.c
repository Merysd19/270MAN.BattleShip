#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MOVES_COUNT 5
#define SHIPS_COUNT 4
#define GRID_SIZE 10

// stucts
typedef struct ship
{
    char name[20];
    int remainingHits; // number of remaining hits until the ship sinks
} Ship;

typedef struct move
{
    char name[50];
    int countAvailable;
    int shipsSunkToUnlock;
} Move;

typedef struct cell // Define and typedef Cell the same time
{
    int col;
    int row;
    struct cell *next; // Correct self-referencing with struct cell*
} Cell;

typedef struct smokedCells
{
    Cell *head;
} SmokedCells;

// player:
typedef struct player
{
    char name[100];
    int **grid;
    int shipsSunk;
    Ship *ships;
    Move *moves;
    SmokedCells *smokedCells;
    // BOT
    int isBot;
    int difficulty;
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

int chooseMode();

Player createPlayer();

Ship *createShips();

Move *createMoves();

SmokedCells *createSmokedList();

// Added for Bot
Player createBotPlayer(int difficulty); // WORKS

int randomCoordinate(int upperBound);

// int validRowOrCol(Player *opponent, int row, int col); //HELPER FOR TORPEDO

// grid:
int **createGrid();

void displayGrid(Player *player);

// ship placement:
void placeShips(Player *player);

void placeShip(Player *player, int shipSize);

int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation);

void botPlaceShip(Player *player, int shipSize);

int botShipOverlap(Player *player, int shipSize, int row, int col, int isVertical);

char *getShipName(int i); // i: 2->5

void clearInputBuffer();

// game play:
void takeTurn(Player *player, Player *opponent); // modified for bot

void displayAvailableMoves(Player *player, Player *opponent);

int makeMove(Player *player, Player *opponent); // modified for bot

int gameOver(Player *opponent, Player *player);

// moves + their helper functions:
int fire(Player *player, Player *opponent); // modified for bot

int radarSweep(Player *player, Player *opponent); // modified for bot

int smokeScreen(Player *player, Player *opponent); // modified for bot

int artillery(Player *player, Player *opponent); // modified for bot

int torpedo(Player *player, Player *opponent); // modified for bot

int checkAvailable(Player *player, int move);

int botCheckAvailable(Player *player, int moveChosen);

int validTopLeftCoordinate(int row, int col);

Cell *createSmokedCell(int col, int row);

int inSmokedList(Player *opponent, int row, int col);

void updateGameState(Player *opponent, Player *player);

void updateMoves(Player *opponent, Player *player);

void checkOneRoundMoves(Player *player, int move);

int strcmpIgnoreNull(char *str1, char *str2);

void freeAll(Player player);

// tracking difficulty level
int mode;

int main()
{

    /*-------------------------------------------------Game Setup and Initialization-------------------------------------------------------*/

    srand(time(NULL)); // seed the random number generator with current time

    // player chooses: player vs player, OR player vs bot

    Player player1 = createPlayer();
    // if bot, player2 is a bot
    Player player2;

    // read game mode:

    mode = chooseMode();
    printf("\n");

    // read players names:
    // Player1
    printf("Please enter your names!\nPlayer1: ");
    scanf_s(" %99s", player1.name, 100);
    // Player2 :Bot or Human?
    printf("Player vs Bot (1) OR Player vs Player (0): "); 
    int isBot;
    scanf("%d", &isBot);

    if (isBot)
    {
        printf("Choose bot difficulty (0: Easy, 1: Medium, 2: Hard): ");
        int botDifficulty;
        scanf("%d", &botDifficulty);
        player2 = createBotPlayer(botDifficulty);
    }
    else
    {
        player2 = createPlayer();
        printf("Player2: ");
        scanf_s(" %99s", player2.name, 100);
    }

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
    printf("%s will start! Press enter to proceed.\n", startingPlayer->name);
    printf("\n");
    getchar();
    system("cls");
    placeShips(startingPlayer);
    printf("Now is %s's turn! Press enter to proceed.\n", otherPlayer->name);
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
        if (gameOver(otherPlayer, startingPlayer))
            break;
        takeTurn(otherPlayer, startingPlayer);
        if (gameOver(startingPlayer, otherPlayer))
            break;
    }

    // free memory allocated for the players:
    freeAll(player1);
    freeAll(player2);

    return 0;
}

/*-------------------------------------------------Game Setup and Initialization-------------------------------------------------------*/
int chooseMode()
{
    char input[10]; // Buffer for input
    int mode;

    printf("Welcome to MAN.BattleShip!\n\nPlease choose the tracking difficulty level by entering '0' for easy and '1' for hard: ");

    while (1) // Loop until valid input is received
    {
        // Read the input
        scanf("%s", input); // Read input as a string

        // Validate if the input is a number
        int validInput = 1; // Flag to check if input is valid
        for (size_t i = 0; i < strlen(input); i++)
        {
            if (!isdigit(input[i])) // Check if each character is a digit
            {
                validInput = 0; // Not a valid number
                break;
            }
        }

        // If the input is valid and a number
        if (validInput)
        {
            mode = atoi(input); // Convert string to integer

            // Check if the move is either 0 or 1
            if (mode == 0 || mode == 1)
            {
                return mode; // Valid input received, return the difficulty level
            }
            else
            {
                printf("\nInvalid input! Please choose either '0' for easy or '1' for hard : ");
            }
        }
        else
        {
            printf("\nInvalid input! Please enter a valid number (0 or 1): ");
        }
    }
}
Player createPlayer()
{
    Player player;
    player.grid = createGrid();
    player.shipsSunk = 0;
    player.ships = createShips();
    player.moves = createMoves();
    player.smokedCells = createSmokedList();
    player.isBot = 0;       // Default to human player
    player.difficulty = -1; // Not applicable for human player
    return player;
}
/*-----------------------------------------------------------------BOT----------------------------------------------------------------*/
Player createBotPlayer(int difficulty)
{
    Player bot = createPlayer();
    strcpy(bot.name, "Bot");
    bot.isBot = 1;               // Mark as bot
    bot.difficulty = difficulty; // Set bot difficulty
    // No need to allocate grid again since createPlayer() already does it
    return bot;
}

int randomCoordinate(int upperBound)
{
    return rand() % upperBound;
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
    const int valueToUnlock[MOVES_COUNT] = {0, 0, 1, 1, 3};

    for (int i = 0; i < MOVES_COUNT; i++)
    {
        strcpy(moves[i].name, names[i]);
        moves[i].countAvailable = counts[i];
        moves[i].shipsSunkToUnlock = valueToUnlock[i];
    }
    return moves;
}

SmokedCells *createSmokedList()
{
    SmokedCells *smokedCells = (SmokedCells *)malloc(sizeof(SmokedCells));
    if (smokedCells == NULL)
    {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    smokedCells->head = NULL;
    return smokedCells;
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
    if (!(player->isBot))
    {
        // instructions:
        printf("These are your ships and their sizes: \ncarrier, 5 cells\nbattleship, 4 cells\ndestroyer, 3 cells\nsubmarine, 2 cells\n");
        printf("\n");
        printf("Please provide the coordinates and orientation to place each ship on the grid (e.g. B3, Horizontal).\n");
        printf("The coordinates indicate the starting position of the ship. Horizontal orientation moves from left to right, and vertical orientation moves from top to bottom.\n");
        printf("Follow this exact format: \ncoordinates: ColumnRow  (CapitalLetter[A->J]Number[1->10]) (e.g. B3)\norientation: 'H' for horizontal, 'V' for vertical\n");
        printf("\n");
        printf("Your input:\n");
    }

    for (int shipSize = 5; shipSize > 1; shipSize--)
    {
        if (!(player->isBot))
        {
            placeShip(player, shipSize);
            getchar();
        }
        else
        {
            botPlaceShip(player, shipSize);
        }
    }

    printf("Done placing %s's ships! Press enter to proceed\n", player->name);

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
    printf("coordinates (e.g. B3): ");
    scanf(" %c%d", &cl, &rw);
    clearInputBuffer();
    printf("orientation (H/V): ");
    scanf(" %c", &orientation);
    printf("\n");

    // map input to coordinates on the grid with 0-based index:
    int col = cl - 'A';
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
    else // try again with same ship
    {
        placeShip(player, shipSize);
    }
}

int canPlaceShip(Player *player, int shipSize, int row, int col, char orientation)
{
    if (col < 0 || col > 9 || row < 0 || row > 9 || (orientation != 'H' && orientation != 'V'))
    {
        printf("Invalid input format! Please try again while following the specified input format directions:\n\n");
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

void botPlaceShip(Player *player, int shipSize)
{
    int row, col, isVertical;

    do
    {
        isVertical = rand() % 2;
        if (isVertical)
        {
            row = randomCoordinate(GRID_SIZE - shipSize + 1);
            col = randomCoordinate(GRID_SIZE);
        }
        else
        {
            row = randomCoordinate(GRID_SIZE);
            col = randomCoordinate(GRID_SIZE - shipSize + 1);
        }
    } while (botShipOverlap(player, shipSize, row, col, isVertical));

    if (isVertical)
    {
        for (int j = row; j < row + shipSize; j++)
        {
            player->grid[j][col] = shipSize;
        }
    }
    else
    {
        for (int j = col; j < col + shipSize; j++)
        {
            player->grid[row][j] = shipSize;
        }
    }
}

int botShipOverlap(Player *player, int shipSize, int row, int col, int isVertical)
{
    if (isVertical)
    {
        for (int j = row; j < row + shipSize; j++)
        {
            if (player->grid[j][col] != empty)
            {
                return 1;
            }
        }
    }
    else
    {
        for (int j = col; j < col + shipSize; j++)
        {
            if (player->grid[row][j] != empty)
            {
                return 1;
            }
        }
    }
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

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
/*-----------------------------------------------------------Game Play-----------------------------------------------------------------*/

void takeTurn(Player *player, Player *opponent)
{
    if (player->isBot)
    {
        // Bot's turn logic
        printf("%s taking turn...\n", player->name);
    }
    else
    {
        printf("%s's Turn! Press enter to proceed \n", player->name);
        getchar();
        system("cls");

        printf("%s's current grid: \n\n", opponent->name);
        displayGrid(opponent);

        printf("\nAvailable moves: \n\n");
        displayAvailableMoves(player, opponent);
    }
    if (makeMove(player, opponent))
    { // move complete, player did not lose their turn
        updateGameState(opponent, player);
        printf("\n%s's updated grid: \n\n", opponent->name);
        displayGrid(opponent);
    }
    else
    {

        getchar();
    }

    getchar();
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
                printf("Input format: identifier: '1', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n\n");
            }
            if (i == 2)
            {
                printf("SMOKE SCREEN, obscures a 2x2 area of your grid by hiding it from radar sweeps.\n");
                printf("Input format: identifier: '2', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n\n");
            }
            if (i == 3)
            {
                printf("ARTILLERY, an attack move that works similarly to FIRE but targets a 2x2 area.\n");
                printf("Input format: identifier: '3', top-left coordinate (e.g. B3, to target B3, B4, C3, C4)\n\n");
            }
            if (i == 4)
            {
                printf("TORPEDO, powerful attack that targets an entire row or column.\n");
                printf("Input format: identifier: '4', either row number (1->10) or column letter (A->J).\n\n");
            }
        }
    }
    printf("\n");
}

int makeMove(Player *player, Player *opponent) // handle inputs that are not numbers
{
    // Bot logic
    if (player->isBot)
    {
        int moveChosen = -1; // Move chosen by the bot
        int result = 0;

        // Ensure bot chooses a valid move
        do
        {
            // CHOOSE THE MOVE DIFFERENTLY
            //what move were choosing depends on the difficulty
            moveChosen = rand() % MOVES_COUNT; // Randomly choose a move
        } while (!botCheckAvailable(player, moveChosen)); // Ensure the move is valid

        // Execute the chosen move for Easy Bot
        switch (moveChosen)
        {
        case 0:                               // FIRE logic for Easy Bot
            printf("Bot performing Fire.\n"); // Print bot's move
            result = fire(player, opponent);  // Perform the FIRE move
            break;

        case 1: // RADAR SWEEP (Placeholder for Easy Bot Logic)
            printf("Bot performs Radar Sweep.\n");
            result = radarSweep(player, opponent);
            break;

        case 2: // SMOKE SCREEN (Placeholder for Easy Bot Logic)
            printf("Bot uses Smoke Screen.\n");
            result = smokeScreen(player, opponent);
            break;

        case 3: // ARTILLERY (Placeholder for Easy Bot Logic)
            printf("Bot fires Artillery.\n");
            result = artillery(player, opponent);
            break;

        case 4: // TORPEDO (Placeholder for Easy Bot Logic)
            printf("Bot fires Torpedo.\n");
            result = torpedo(player, opponent);
            break;

        default:
            printf("Bot failed to make a valid move.\n");
            return 0; // Skip turn if no valid move is made
        }

        if (result)
        {
            player->moves[moveChosen].countAvailable--; // Decrement move availability
        }

        return result; // Return whether the bot successfully made a move
    }

    // Human player logic
    char input[10];
    int move;
    int result = 0;

    while (1)
    {
        printf("Enter the identifier for your chosen move: ");
        scanf("%s", input); // Read input as a string

        // Check if the input is a valid number
        int validInput = 1; // Flag to check if input is valid
        for (size_t i = 0; i < strlen(input); i++)
        {
            if (!isdigit(input[i]))
            {
                validInput = 0; // Not a valid number
                break;
            }
        }

        if (validInput)
        {
            move = atoi(input); // Convert string to integer

            // Check if the move is within the valid range
            if (move >= 0 && move < MOVES_COUNT)
            {
                checkOneRoundMoves(player, move);

                if (!checkAvailable(player, move))
                {
                    return 0; // Invalid move available
                }

                switch (move)
                {
                case 0:
                    result = fire(player, opponent);
                    break;
                case 1:
                    result = radarSweep(player, opponent);
                    break;
                case 2:
                    result = smokeScreen(player, opponent);
                    break;
                case 3:
                    result = artillery(player, opponent);
                    break;
                case 4:
                    result = torpedo(player, opponent);
                    break;
                default:
                    break;
                }

                if (result)
                {
                    player->moves[move].countAvailable--;
                }

                break; // Exit the loop after a valid move
            }
        }

        // If input is not a valid number or not in range
        printf("\nInvalid input! Please choose again from the list of available moves using the specified format:\n\n");
    }

    return 1; // Return success
}

int fire(Player *player, Player *opponent)
{
    int row = -1, col = -1;

    if (player->isBot)
    {
        // Easy Bot Logic
        if (player->difficulty == 0)
        {
            // select rand cell thats Unused
            do
            {
                row = randomCoordinate(GRID_SIZE);
                col = randomCoordinate(GRID_SIZE);
            } while (opponent->grid[row][col] == hit || opponent->grid[row][col] == miss);
        }
        // Medium Bot Logic
        else if (player->difficulty == 1)
        {
            // to be implemented
        }
        // Hard Bot Logic
        else if (player->difficulty == 2)
        {
            // to be implemented
        }
    }
    else
    {
        // Human Player Logic
        char cl;
        int rw;

        printf("\nEnter coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        row = rw - 1;
        col = cl - 'A';

        if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE)
        {
            printf("\nInvalid coordinates! You lose your turn :(\n");
            return 0;
        }
    }

    // Fire at the chosen coordinates
    int cell = opponent->grid[row][col];
    if (cell > 1)
    {
        opponent->grid[row][col] = hit;
        printf("\nResult: hit!\n");

        // Decrement the ship's remaining hits
        for (int j = 0; j < SHIPS_COUNT; j++)
        {
            if (strcmpIgnoreNull(opponent->ships[j].name, getShipName(cell)))
            {
                opponent->ships[j].remainingHits--;
            }
        }
    }
    else
    {
        if (cell != hit)
        {
            opponent->grid[row][col] = miss;
        }
        printf("\nResult: miss!\n");
    }
    return 1;
}

int radarSweep(Player *player, Player *opponent)
{
    int row = -1, col = -1;

    if (player->isBot)
    {
        // Easy Bot Logic: Randomly select a valid top-left coordinate
        if (player->difficulty == 0)
        {
            row = randomCoordinate(GRID_SIZE - 1);
            col = randomCoordinate(GRID_SIZE - 1);
        }
        // Medium Bot Logic
        else if (player->difficulty == 1)
        {
            // to be implemented
        }
        // Hard Bot Logic
        else if (player->difficulty == 2)
        {
            // to be implemented
        }
    }
    else
    {
        // Human Player Logic
        char cl;
        int rw;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        row = rw - 1;
        col = cl - 'A';

        if (!validTopLeftCoordinate(row, col))
        {
            printf("\nInvalid coordinates! You lose your turn :(\n");
            return 0;
        }
    }

    // Radar Sweep Logic
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int gridSymbol = opponent->grid[row + i][col + j];
            if (gridSymbol != hit && gridSymbol != miss && gridSymbol != empty)
            {
                if (inSmokedList(opponent, row + i, col + j) == 0)
                {
                    if (!(player->isBot))
                    {
                        printf("\nResult: enemy ships found!\n");
                    }
                    return 1;
                }
            }
        }
    }
    if (!(player->isBot))
    {
        printf("\nResult: no enemy ships found!\n");
    }
    return 1;
}

int smokeScreen(Player *player, Player *opponent)
{
    int row = -1, col = -1;

    if (player->isBot)
    {
        // Easy Bot Logic: Randomly select a valid top-left coordinate
        if (player->difficulty == 0)
        {
            row = randomCoordinate(GRID_SIZE - 1);
            col = randomCoordinate(GRID_SIZE - 1);
        }
        // Medium Bot Logic (Placeholder)
        else if (player->difficulty == 1)
        {
            // implement
        }
        // Hard Bot Logic (Placeholder)
        else if (player->difficulty == 2)
        {
            // implement
        }
    }
    else
    {
        // Human Player Logic
        char cl;
        int rw;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        row = rw - 1;
        col = cl - 'A';

        if (!validTopLeftCoordinate(row, col))
        {
            printf("\nInvalid coordinates! You lose your turn :(\n");
            return 0;
        }
    }

    // Perform Smoke Screen Logic
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int gridSymbol = player->grid[row + i][col + j];
            if (gridSymbol != hit && gridSymbol != miss && gridSymbol != empty)
            {
                if (inSmokedList(player, row + i, col + j) == 0)
                {
                    Cell *newCell = createSmokedCell(col + j, row + i);
                    newCell->next = player->smokedCells->head;
                    player->smokedCells->head = newCell;
                }
            }
        }
    }
    if (!(player->isBot))
    {
        printf("\nSMOKE SCREEN performed! Press enter to proceed \n");
        getchar();
        system("cls");
    }
    return 1;
}

int artillery(Player *player, Player *opponent)
{
    int row = -1, col = -1;

    if (player->isBot)
    {
        // Easy Bot Logic: Randomly select a valid top-left coordinate
        if (player->difficulty == 0)
        {
            do
            {
                row = randomCoordinate(GRID_SIZE - 1);
                col = randomCoordinate(GRID_SIZE - 1);
            } while (opponent->grid[row][col] == hit || opponent->grid[row][col] == miss);
        }
        // Medium Bot Logic (Placeholder)
        else if (player->difficulty == 1)
        {
            // implement
        }
        // Hard Bot Logic (Placeholder)
        else if (player->difficulty == 2)
        {
            // implement
        }
    }
    else
    {
        // Human Player Logic
        char cl;
        int rw;

        printf("\nEnter top-left coordinate (e.g. B3): ");
        scanf(" %c%d", &cl, &rw);

        row = rw - 1;
        col = cl - 'A';

        if (!validTopLeftCoordinate(row, col))
        {
            printf("\nInvalid coordinates! You lose your turn :(\n");
            return 0;
        }
    }

    // Artillery Logic
    int h = 0; // Number of hits
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int gridSymbol = opponent->grid[row + i][col + j];
            if (gridSymbol > 1)
            {
                opponent->grid[row + i][col + j] = hit;
                h++;
                for (int k = 0; k < SHIPS_COUNT; k++)
                {
                    if (strcmpIgnoreNull(opponent->ships[k].name, getShipName(gridSymbol)))
                    {
                        opponent->ships[k].remainingHits--;
                    }
                }
            }
            else
            {
                if (gridSymbol != hit)
                {
                    opponent->grid[row + i][col + j] = miss;
                }
            }
        }
    }
    if (h > 0)
    {
        printf("\nResult: hit!\n");
    }
    else
    {
        printf("\nResult: miss!\n");
    }
    return 1;
}

int torpedo(Player *player, Player *opponent)
{
    int row = -1;
    int col = -1;

    if (player->isBot)
    {
        // Easy Bot Logic: Randomly choose a valid row or column
        if (player->difficulty == 0)
        {
            int isRow = rand() % 2; // Randomly choose between row or column
            if (isRow)
            {
                row = randomCoordinate(GRID_SIZE);
            }
            else
            {
                col = randomCoordinate(GRID_SIZE);
            }
        }
        // Medium Bot Logic (Placeholder)
        else if (player->difficulty == 1)
        {
            // implement
        }
        // Hard Bot Logic (Placeholder)
        else if (player->difficulty == 2)
        {
            // implement
        }

        printf("%s performs torpedo at %s %c\n",
               player->name,
               (row != -1) ? "row" : "column",
               (row != -1) ? row + '1' : col + 'A'); // Debugging output for bot
    }
    else
    {
        // Human Player Logic
        char input;
        printf("\nEnter row (e.g. 3) or column (e.g. B): ");
        scanf(" %c", &input);

        // Validate input
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
            printf("\nInvalid coordinates! You lose your turn :(\n");
            getchar();
            return 0;
        }
    }

    // Perform Torpedo Logic
    int h = 0; // Number of hits

    if (col == -1)
    { // Target a row
        for (int i = 0; i < GRID_SIZE; i++)
        {
            int gridSymbol = opponent->grid[row][i];
            if (gridSymbol > 1)
            {
                opponent->grid[row][i] = hit;
                h++;
                for (int k = 0; k < SHIPS_COUNT; k++)
                {
                    if (strcmpIgnoreNull(opponent->ships[k].name, getShipName(gridSymbol)))
                    {
                        opponent->ships[k].remainingHits--;
                    }
                }
            }
            else
            {
                if (gridSymbol != hit)
                {
                    opponent->grid[row][i] = miss;
                }
            }
        }
    }
    else
    { // Target a column
        for (int i = 0; i < GRID_SIZE; i++)
        {
            int gridSymbol = opponent->grid[i][col];
            if (gridSymbol > 1)
            {
                opponent->grid[i][col] = hit;
                h++;
                for (int k = 0; k < SHIPS_COUNT; k++)
                {
                    if (strcmpIgnoreNull(opponent->ships[k].name, getShipName(gridSymbol)))
                    {
                        opponent->ships[k].remainingHits--;
                    }
                }
            }
            else
            {
                if (gridSymbol != hit)
                {
                    opponent->grid[i][col] = miss;
                }
            }
        }
    }

    if (h > 0)
    {
        printf("\nResult: hit!\n");
    }
    else
    {
        printf("\nResult: miss!\n");
    }
    return 1;
}
// helper funct for torpedo
// int validRowOrCol(Player *opponent, int row, int col) {
//     if (row != -1) { // Check row validity
//         for (int i = 0; i < GRID_SIZE; i++) {
//             if (opponent->grid[row][i] != hit && opponent->grid[row][i] != miss) {
//                 return 1; // Row is valid
//             }
//         }
//     } else if (col != -1) { // Check column validity
//         for (int i = 0; i < GRID_SIZE; i++) {
//             if (opponent->grid[i][col] != hit && opponent->grid[i][col] != miss) {
//                 return 1; // Column is valid
//             }
//         }
//     }
//     return 0; // Row or column is invalid
// }

void checkOneRoundMoves(Player *player, int move)
{

    int movesToCheck[2] = {3, 4};

    for (int i = 0; i < 2; i++)
    {
        if (move != movesToCheck[i] && player->moves[movesToCheck[i]].countAvailable == 1)
        {
            player->moves[movesToCheck[i]].countAvailable--;
        }
    }
}

int checkAvailable(Player *player, int move)
{
    if (player->moves[move].countAvailable == 0)
    {
        printf("\nOops, you don't have an available %s move! You lost your turn :(\n", player->moves[move].name);
        getchar();
        return 0;
    }
    return 1;
}

int botCheckAvailable(Player *player, int moveChosen)
{
    if (player->moves[moveChosen].countAvailable == 0)
    {
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
    printf("\nInvalid coordinates! You lost your turn :(\n");
    getchar();
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

void updateGameState(Player *opponent, Player *player)
{

    for (int i = 0; i < SHIPS_COUNT; i++)
    {
        if (opponent->ships[i].remainingHits == 0)
        {
            opponent->ships[i].remainingHits--;
            opponent->shipsSunk++;
            printf("\nOne of %s's ships, a %s, has been sunk!\n", opponent->name, getShipName(i + 2));
            updateMoves(opponent, player);
        }
    }
}

void updateMoves(Player *opponent, Player *player)
{
    for (int j = 0; j < MOVES_COUNT; j++)
    {
        if (player->moves[j].shipsSunkToUnlock <= opponent->shipsSunk)
        {
            player->moves[j].countAvailable++;
        }
    }
}

int gameOver(Player *opponent, Player *player)
{

    if (opponent->shipsSunk == 4)
    {
        printf("All of %s's ships have been sunk! %s wins, congrats :)", opponent->name, player->name);
        getchar();
        return 1;
    }
    return 0;
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
    Cell *current = player.smokedCells->head;
    while (current != NULL)
    {
        Cell *temp = current;
        current = current->next;
        free(temp);
    }
}

int strcmpIgnoreNull(char *str1, char *str2)
{
    int i = 0, j = 0;

    while (str1[i] != '\0' || str2[j] != '\0')
    {
        while (str1[i] == '\0')
            i++;
        while (str2[j] == '\0')
            j++;

        if (str1[i] != str2[j])
        {
            return 0;
        }

        i++;
        j++;
    }

    return 1;
}
