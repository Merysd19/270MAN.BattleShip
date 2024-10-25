#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
//-----------------------------------------------Part 1: Game Setup--------------------------------------------------
// Name and Grid of each Player 
typedef struct player {
    char name[100]; 
    int **grid;
    int radarSweepsRemaining;
    int smokeScreensRemaining;
} Player;

// Cell states
enum cellStates {
    empty = -1,
    miss,
    hit,
    submarine,
    destroyer,
    battleship,
    carrier
};

// Functions 
char *getShipName(int i);
int **createGrid();
void displayGrid(Player *player);
void placeShips(Player *player);
int canPlaceShip(Player *player, int row, int col, int size, char orientation);
void playerTurn(Player *attacker, Player *defender);
int fire(Player *attacker, Player *defender, int row, int col);
void radarSweep(Player *player, int row, int col);
void smokeScreen(Player *player, int row, int col);
void checkGameOver(Player *player1, Player *player2);
void clearGrid(Player *player);

// Global variable mode to track game difficulty 
int mode;

int main() {
    srand(time(NULL)); // Seed the random number generator with current time

    Player player1, player2;
    player1.grid = createGrid();
    player2.grid = createGrid();
    player1.radarSweepsRemaining = 3;
    player1.smokeScreensRemaining = 1;
    player2.radarSweepsRemaining = 3;
    player2.smokeScreensRemaining = 1;

    // Input Mode 
    printf("Welcome to MAN.BattleShip! Please choose the tracking difficulty level by entering '0' for easy and '1' for hard: ");
    scanf("%d", &mode);

    // input Player names 
    printf("Please enter your names!\nPlayer1: ");
    scanf("%99s", player1.name);
    printf("Player2: ");
    scanf("%99s", player2.name);

    displayGrid(&player1);
    displayGrid(&player2);

    // Placing ships for players
    printf("Placing your ships: \n%s will start!\n", player1.name);
    placeShips(&player1);
    printf("Now is %s's turn!\n", player2.name);
    placeShips(&player2);

    // Game Loop
    while (1) {
        playerTurn(&player1, &player2);
        checkGameOver(&player1, &player2);
        playerTurn(&player2, &player1);
        checkGameOver(&player1, &player2);
    }

    // Free memory allocated for the grids
    clearGrid(&player1);
    clearGrid(&player2);

    return 0;
}

char *getShipName(int i) {
    switch (i) {
        case 5: return "carrier";
        case 4: return "battleship";
        case 3: return "destroyer";
        case 2: return "submarine";
        default: return "unknown";
    }
}

int **createGrid() {
    int **grid = malloc(sizeof(int *) * 10);
    if (grid == NULL) {
        printf("Failed to allocate needed memory\n");
        exit(1);
    }
    for (int i = 0; i < 10; i++) {
        grid[i] = malloc(sizeof(int) * 10);
        if (grid[i] == NULL) {
            printf("Failed to allocate needed memory\n");
            exit(1);
        }
    }
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            grid[i][j] = empty; // Initialize the grid to empty
    return grid;
}

void displayGrid(Player *player) {
    printf("%s:\n   A B C D E F G H I J\n", player->name);
    for (int i = 0; i < 10; i++) {
        printf("%2d", i + 1);
        for (int j = 0; j < 10; j++) {
            char c;
            switch (player->grid[i][j]) {
                case miss: c = (mode == 0) ? 'o' : '~'; break;
                case hit: c = '*'; break;
                default: c = '~'; break; // empty or has a ship
            }
            printf(" %c", c);
        }
        printf("\n");
    }
}

int canPlaceShip(Player *player, int row, int col, int size, char orientation) {
    if (orientation == 'h') {
        if (col + size > 10) return 0; // Out of bounds
        for (int j = col; j < col + size; j++) {
            if (player->grid[row][j] != empty) return 0; // Overlapping
        }
    } else if (orientation == 'v') {
        if (row + size > 10) return 0; // Out of bounds
        for (int j = row; j < row + size; j++) {
            if (player->grid[j][col] != empty) return 0; // Overlapping
        }
    }
    return 1; // Valid placement
}

void placeShips(Player *player) {
    printf("These are your ships and their sizes: \ncarrier, 5 cells\nbattleship, 4 cells\ndestroyer, 3 cells\nsubmarine, 2 cells\n");
    printf("Please provide the coordinates and orientation to place each ship on the grid (e.g. B3, horizontal).\n");
    printf("The coordinates indicate the starting position of the ship. Horizontal orientation moves from left to right, and vertical orientation moves from top to bottom.\n");
    printf("Follow this exact format: \ncoordinates: column: CAPITAL_LETTER (A->J), row: number (1->10)\norientation: 'h' for horizontal, 'v' for vertical\n");

    for (int i = 5; i > 1; i--) {
        char *name = getShipName(i);
        int row;
        char col, orientation;
        printf("(ship name: %s, ship size: %d)\n", name, i);
        printf("column (A->J): ");
        scanf(" %c", &col);
        printf("row (1->10): ");
        scanf("%d", &row);
        printf("orientation (h/v): ");
        scanf(" %c", &orientation);
        int hStartingIndex = col - 'A'; // Convert char to index
        int vStartingIndex = row - 1;   // Convert row from 1-based to 0-based

        // Validate ship placement
        if (hStartingIndex >= 0 && hStartingIndex <= 9 && vStartingIndex >= 0 && vStartingIndex <= 9 && (orientation == 'h' || orientation == 'v')) {
            if (canPlaceShip(player, vStartingIndex, hStartingIndex, i, orientation)) {
                // Place the ship
                if (orientation == 'h') {
                    for (int j = hStartingIndex; j < hStartingIndex + i; j++) {
                        player->grid[vStartingIndex][j] = i; // Place the ship
                    }
                } else { // orientation == 'v'
                    for (int j = vStartingIndex; j < vStartingIndex + i; j++) {
                        player->grid[j][hStartingIndex] = i; // Place the ship
                    }
                }
            } else {
                printf("Error: chosen coordinates overlap with another ship or extend beyond the grid! Try again:\n");
                i++; // Stay on the same ship
            }
        } else {
            printf("Invalid input format! Please try again while following the specified input format directions:\n");
            i++; // Stay on the same ship
        }
    }

    printf("Done placing your ships! Press enter to proceed\n");
    getchar(); // Clear the newline from the input buffer
    getchar(); // Wait for the user to press enter
    system("cls"); // Clear the screen (Use "clear" for Unix-based systems)
}

void playerTurn(Player *attacker, Player *defender) {
    char action;
    printf("%s's turn! Choose your action:\n", attacker->name);
    printf("Enter 'F' to fire, 'R' for radar sweep, or 'S' for smoke screen: ");
    scanf(" %c", &action);

    int row, col;
    char colChar;

    switch (action) {
        case 'F': // Fire action
            printf("Enter coordinates to fire (e.g., B3): ");
            scanf(" %c%d", &colChar, &row);
            col = colChar - 'A'; // Convert to index
            if (fire(attacker, defender, row - 1, col) == hit) { // Adjusting for 0-based index
                printf("Hit!\n");
                attacker->smokeScreensRemaining = 1; // Award smoke screen after a hit
            } else {
                printf("Miss!\n");
            }
            break;

        case 'R': // Radar Sweep action
            if (attacker->radarSweepsRemaining > 0) {
                printf("Enter coordinates for radar sweep (e.g., B3): ");
                scanf(" %c%d", &colChar, &row);
                col = colChar - 'A'; // Convert to index
                radarSweep(attacker, row - 1, col);
                attacker->radarSweepsRemaining--;
            } else {
                printf("No radar sweeps remaining!\n");
            }
            break;

        case 'S': // Smoke Screen action
            if (attacker->smokeScreensRemaining > 0) {
                printf("Enter coordinates for smoke screen (e.g., B3): ");
                scanf(" %c%d", &colChar, &row);
                col = colChar - 'A'; // Convert to index
                smokeScreen(attacker, row - 1, col);
                attacker->smokeScreensRemaining--;
            } else {
                printf("No smoke screens remaining!\n");
            }
            break;

        default:
            printf("Invalid action! Please choose again.\n");
            playerTurn(attacker, defender); // Repeat the turn
            break;
    }
}

int fire(Player *attacker, Player *defender, int row, int col) {
    if (defender->grid[row][col] > 1) { // Check for ship
        defender->grid[row][col] = hit; // Mark as hit
        return hit;
    }
    defender->grid[row][col] = miss; // Mark as miss
    return miss;
}

void radarSweep(Player *player, int row, int col) {
    printf("Performing radar sweep at %c%d...\n", 'A' + col, row + 1);
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < 10 && j >= 0 && j < 10) {
                if (player->grid[i][j] > 1) {
                    printf("Ship detected at %c%d!\n", 'A' + j, i + 1);
                }
            }
        }
    }
}

void smokeScreen(Player *player, int row, int col) {
    printf("Deploying smoke screen at %c%d...\n", 'A' + col, row + 1);
    // Logic to block the opponent's view (implement based on game rules)
}

void checkGameOver(Player *player1, Player *player2) {
    int player1Ships = 0;
    int player2Ships = 0;

    // Count remaining ships for player1
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (player1->grid[i][j] > 1) {
                player1Ships++;
            }
        }
    }

    // Count remaining ships for player2
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (player2->grid[i][j] > 1) {
                player2Ships++;
            }
        }
    }

    if (player1Ships == 0) {
        printf("%s wins!\n", player2->name);
        exit(0); // End game
    } else if (player2Ships == 0) {
        printf("%s wins!\n", player1->name);
        exit(0); // End game
    }
}

void clearGrid(Player *player) {
    for (int i = 0; i < 10; i++) {
        free(player->grid[i]); // Free each row
    }
    free(player->grid); // Free the array of rows
}
