// CSE 101 - MELODY GUESSING SYSTEM
// Functions: Game flow, random selection, main menu interface

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

typedef struct {
    int id;
    char song_name[50];
    char artist[50];
    int melody_duration;
} Song;

typedef struct {
    int current_round;
    int total_rounds;
    int player1_score;
    int player2_score;
    int melody_duration;
    int difficulty_level;
    Song current_song;
} GameState;

typedef struct {
    int player1_guess;
    int player2_guess;
    int correct_answer;
    Song song;
} RoundResult;

GameState game_state = {
    .current_round = 0,
    .total_rounds = 3,
    .player1_score = 0,
    .player2_score = 0,
    .melody_duration = 10000,  // Default: 10 seconds
    .difficulty_level = 1
};

Song song_database[5];
int song_count = 0;

// External serial port from serial_protocol.c
extern int serial_port;
extern int open_serial_port(const char *port);
extern int close_serial_port(void);

// Menu Functions
void display_main_menu(void);
void display_game_menu(void);

// Game Logic
void start_new_game(void);
void play_round(int round);
void get_player_responses(RoundResult *result);
void parse_arduino_response(const char *buffer, RoundResult *result, int *p1_recv, int *p2_recv);
void display_round_results(RoundResult *result, int round);
void display_final_results(void);
void process_round_data(RoundResult *result);

// Game Settings
void change_difficulty(void);
void view_settings(void);
void reset_game(void);
void display_scoreboard(void);

// Database Functions
void load_song_database(void);
Song select_random_song(void);

// Arduino Functions
void initialize_arduino_connection(void);
void test_arduino_connection(void);
void send_to_arduino(const char *message);
int read_from_arduino(char *buffer, int size, int timeout_seconds);

int main() {
    int choice;
    
    srand(time(NULL));
    
    printf("\n\n");
    printf("  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                              ║\n");
    printf("  ║          🎵  MELODY BATTLE - ADMIN CONSOLE  🎵              ║\n");
    printf("  ║                                                              ║\n");
    printf("  ║            C Logic & Game Control Module v1.0               ║\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
    
    // Initialize Arduino connection
    initialize_arduino_connection();
    
    // Load song database
    load_song_database();
    
    // Main menu loop
    while (1) {
        display_main_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            getchar();  // Clear invalid input
            printf("[!] Invalid input. Please enter a number (1-6).\n");
            continue;
        }
        getchar();  // Clear newline
        
        switch (choice) {
            case 1: 
                start_new_game();           
                break;
            case 2: 
                change_difficulty();        
                break;
            case 3: 
                view_settings();            
                break;
            case 4: 
                display_scoreboard();       
                break;
            case 5: 
                test_arduino_connection();  
                break;
            case 6:
                printf("\n[*] Exiting Admin Console. Goodbye!\n\n");
                if (serial_port >= 0) 
                    close_serial_port();
                return 0;
            default:
                printf("[!] Invalid choice (1-6).\n");
        }
    }
    return 0;
}

void display_main_menu(void) {
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                   📋 ADMIN CONSOLE MENU                      ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  [1] 🎮 Start New Game                                      ║\n");
    printf("  ║  [2] ⚙️  Change Difficulty Level                            ║\n");
    printf("  ║  [3] 📊 View Game Settings                                  ║\n");
    printf("  ║  [4] 🏆 View Scoreboard                                     ║\n");
    printf("  ║  [5] 🔌 Test Arduino Connection                             ║\n");
    printf("  ║  [6] ❌ Exit                                                ║\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n  ➜ Enter your choice (1-6): ");
}

void display_game_menu(void) {
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                  🎯 GAME CONTROL MENU                        ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  Round: %d / %d                                               ║\n", game_state.current_round, game_state.total_rounds);
    printf("  ║  Player 1: %3d pts  │  Player 2: %3d pts                     ║\n", game_state.player1_score, game_state.player2_score);
    printf("  ║                                                              ║\n");
    printf("  ║  [1] ▶️  Start Next Round                                    ║\n");
    printf("  ║  [2] 📊 View Scoreboard                                      ║\n");
    printf("  ║  [3] 🔄 Reset Game                                          ║\n");
    printf("  ║  [4] ⬅️  Back to Main Menu                                   ║\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n  ➜ Enter your choice (1-4): ");
}

void start_new_game(void) {
    printf("\n[*] Starting New Game...\n");
    printf("[*] Select Theme:\n");
    printf("    1. Game Themes\n");
    printf("    2. Traditional\n");
    printf("    3. Pop\n");
    printf("Choice (1-3): ");
    
    int theme;
    if (scanf("%d", &theme) != 1 || theme < 1 || theme > 3)
    {
        getchar();
        printf("[!] Invalid theme selection.\n");
        return;
    }
    getchar();
    
    // Reset game state
    reset_game();
    game_state.current_round = 1;
    printf("[✓] Theme %d selected. Game initialized.\n\n", theme);
    
    // Game loop
    while (game_state.current_round <= game_state.total_rounds)
    {
        display_game_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            getchar();
            printf("[!] Invalid input.\n");
            continue;
        }
        getchar();
        
        switch (choice)
        {
            case 1:
                play_round(game_state.current_round);
                game_state.current_round++;
                break;
            case 2:
                display_scoreboard();
                break;
            case 3:
                printf("[*] Game reset.\n");
                return;
            case 4:
                return;
            default:
                printf("[!] Invalid choice (1-4).\n");
        }
    }
    
    // Display final results
    display_final_results();
    reset_game();
}

void display_final_results(void)
{
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                   🎉 GAME FINISHED! 🎉                       ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  📊 Final Scores:\n");
    printf("  ║    Player 1: %d points\n", game_state.player1_score);
    printf("  ║    Player 2: %d points\n", game_state.player2_score);
    printf("  ║\n");
    
    if (game_state.player1_score > game_state.player2_score)
        printf("  ║  🏆 WINNER: Player 1 🥇\n");
    else if (game_state.player2_score > game_state.player1_score)
        printf("  ║  🏆 WINNER: Player 2 🥇\n");
    else
        printf("  ║  🤝 RESULT: TIE! Equal skills!\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
}

void play_round(int round)
{
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                   🎵 ROUND %d STARTING 🎵                    ║\n", round);
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
    
    // Initialize round data
    RoundResult result = {
        .player1_guess = -1,
        .player2_guess = -1,
        .correct_answer = -1,
        .song = select_random_song()
    };
    
    printf("[✓] Song selected: %s by %s\n", result.song.song_name, result.song.artist);
    printf("[*] Melody duration: %d seconds\n\n", game_state.melody_duration / 1000);
    
    // Signal Arduino to start
    send_to_arduino("START");
    printf("[*] Signal sent to Arduino. Waiting for responses...\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    // Read player guesses
    get_player_responses(&result);
    
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    // Process and display results
    process_round_data(&result);
    display_round_results(&result, round);
}

void get_player_responses(RoundResult *result)
{
    char buffer[256] = {0};
    int p1_received = 0, p2_received = 0;
    int timeout = 30;  // 30 second timeout
    time_t start_time = time(NULL);
    
    printf("[LISTENING] Waiting for player inputs...\n");
    
    while (time(NULL) - start_time < timeout && (!p1_received || !p2_received))
    {
        if (serial_port >= 0)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytes = read(serial_port, buffer, sizeof(buffer) - 1);
            
            if (bytes > 0)
                parse_arduino_response(buffer, result, &p1_received, &p2_received);
        } 
        else 
        {
            // Simulation mode
            if (!p1_received) 
            {
                result->player1_guess = (rand() % 2) + 1;
                printf("[✓] [MOCK] Player 1 guessed: Option %d\n", result->player1_guess);
                p1_received = 1;
            } 
            else if (!p2_received) 
            {
                result->player2_guess = (rand() % 2) + 1;
                result->correct_answer = (rand() % 2) + 1;
                printf("[✓] [MOCK] Player 2 guessed: Option %d\n", result->player2_guess);
                p2_received = 1;
            }
            sleep(1);
        }
        usleep(50000);
    }
    
    // Set defaults for missing data
    if (result->player1_guess == -1) result->player1_guess = 0;
    if (result->player2_guess == -1) result->player2_guess = 0;
    if (result->correct_answer == -1) result->correct_answer = (rand() % 2) + 1;
}

void parse_arduino_response(const char *buffer, RoundResult *result, int *p1_recv, int *p2_recv) 
{
    if (strstr(buffer, "P1_GUESS:") != NULL) 
    {
        sscanf(buffer, "P1_GUESS:%d", &result->player1_guess);
        printf("[✓] Player 1 guessed: Option %d\n", result->player1_guess);
        *p1_recv = 1;
    } 
    else if (strstr(buffer, "P2_GUESS:") != NULL) 
    {
        sscanf(buffer, "P2_GUESS:%d", &result->player2_guess);
        printf("[✓] Player 2 guessed: Option %d\n", result->player2_guess);
        *p2_recv = 1;
    } 
    else if (strstr(buffer, "P1:") != NULL) 
    {
        sscanf(buffer, "P1:%d,P2:%d,CORRECT:%d", &result->player1_guess, &result->player2_guess, &result->correct_answer);
        printf("[✓] Received complete round data from Arduino\n");
        *p1_recv = 1;
        *p2_recv = 1;
    }
}

void process_round_data(RoundResult *result)
{
    // Check Player 1
    if (result->player1_guess == result->correct_answer)
        game_state.player1_score += 10;
    
    // Check Player 2
    if (result->player2_guess == result->correct_answer)
        game_state.player2_score += 10;
}

void display_round_results(RoundResult *result, int round) {
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                  📊 ROUND %d RESULTS                        ║\n", round);
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  🎵 Song: %s by %s\n", result->song.song_name, result->song.artist);
    printf("  ║  ✔ Correct Answer: Option %d\n\n", result->correct_answer);
    
    // Player 1 Result
    printf("  ║  Player 1 ");
    if (result->player1_guess > 0)
    {
        if (result->player1_guess == result->correct_answer)
            printf("(Option %d): ✓ CORRECT! (+10 points)\n", result->player1_guess);
        else
            printf("(Option %d): ✗ WRONG\n", result->player1_guess);
    } else
        printf(": ✗ NO INPUT\n");
    
    // Player 2 Result
    printf("  ║  Player 2 ");
    if (result->player2_guess > 0)
    {
        if (result->player2_guess == result->correct_answer)
            printf("(Option %d): ✓ CORRECT! (+10 points)\n", result->player2_guess);
        else
            printf("(Option %d): ✗ WRONG\n", result->player2_guess);
    }
    else
        printf(": ✗ NO INPUT\n");
    
    printf("  ║\n");
    printf("  ║  📈 Score Update:\n");
    printf("  ║  Player 1: %d pts  │  Player 2: %d pts\n", game_state.player1_score, game_state.player2_score);
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
}

void change_difficulty(void) 
{
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                 🎚️  CHANGE DIFFICULTY LEVEL                 ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  [1] 😊 Easy   (10 seconds)                                 ║\n");
    printf("  ║  [2] 😐 Medium (6 seconds)                                  ║\n");
    printf("  ║  [3] 😰 Hard   (4 seconds)                                  ║\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n  ➜ Select difficulty (1-3): ");
    
    int difficulty;
    if (scanf("%d", &difficulty) != 1)
    {
        getchar();
        printf("  [!] Invalid input.\n");
        return;
    }
    getchar();
    
    const struct {
        int level;
        int duration;
        const char *name;
    } difficulty_map[] = {
        {1, 10000, "EASY"},
        {2, 6000, "MEDIUM"},
        {3, 4000, "HARD"}
    };
    
    for (int i = 0; i < 3; i++)
    {
        if (difficulty == difficulty_map[i].level)
        {
            game_state.difficulty_level = difficulty_map[i].level;
            game_state.melody_duration = difficulty_map[i].duration;
            printf("  [✓] Difficulty set to %s (%d seconds)\n\n", 
                   difficulty_map[i].name, difficulty_map[i].duration / 1000);
            return;
        }
    }
    
    printf("  [!] Invalid difficulty level (1-3).\n\n");
}

void view_settings(void) {
    const char *difficulty_name[] = {"NONE", "EASY", "MEDIUM", "HARD"};
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                    ⚙️  GAME SETTINGS                         ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  Difficulty Level:  %s                                    ║\n", difficulty_name[game_state.difficulty_level]);
    printf("  ║  Melody Duration:   %d seconds                              ║\n", game_state.melody_duration / 1000);
    printf("  ║  Total Rounds:      %d                                       ║\n", game_state.total_rounds);
    printf("  ║  Arduino Status:    %s                                  ║\n", serial_port >= 0 ? "✓ Connected" : "✗ Disconnected");
    printf("  ║  Songs Available:   %d melodies                             ║\n", song_count);
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
}

void display_scoreboard(void) {
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                    🏆 CURRENT SCOREBOARD                     ║\n");
    printf("  ╠══════════════════════════════════════════════════════════════╣\n");
    printf("  ║                                                              ║\n");
    printf("  ║  👤 Player 1:  %-3d points                                   ║\n", game_state.player1_score);
    printf("  ║  👤 Player 2:  %-3d points                                   ║\n", game_state.player2_score);
    printf("  ║                                                              ║\n");
    printf("  ║  Round: %d / %d                                               ║\n", game_state.current_round, game_state.total_rounds);
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
}

void reset_game(void)
{
    game_state.current_round = 0;
    game_state.player1_score = 0;
    game_state.player2_score = 0;
    printf("[✓] Game reset.\n");
}

// =========================================================================
// DATABASE FUNCTIONS
// =========================================================================

void load_song_database(void) {
    // Song database - 5 classic melodies
    const char *songs[][3] = {
        {"Super Mario", "Nintendo", "1"},
        {"Happy Birthday", "Traditional", "2"},
        {"Jingle Bells", "James Pierpont", "3"},
        {"Twinkle Twinkle", "Traditional", "4"},
        {"Mary Had Lamb", "Traditional", "5"}
    };
    
    song_count = 5;
    
    for (int i = 0; i < song_count; i++) {
        song_database[i].id = i + 1;
        strcpy(song_database[i].song_name, songs[i][0]);
        strcpy(song_database[i].artist, songs[i][1]);
    }
    
    printf("[✓] Loaded %d songs from database.\n", song_count);
}

Song select_random_song(void) {
    if (song_count == 0) {
        printf("[!] No songs loaded.\n");
        return song_database[0];
    }
    
    int random_index = rand() % song_count;
    game_state.current_song = song_database[random_index];
    return song_database[random_index];
}

// =========================================================================
// ARDUINO COMMUNICATION FUNCTIONS
// =========================================================================

void initialize_arduino_connection(void) {
    printf("[*] Attempting to connect to Arduino...\n");
    
    const char *ports[] = {"/dev/ttyUSB0", "/dev/ttyACM0", "/dev/ttyUSB1", "/dev/ttyACM1"};
    int port_count = sizeof(ports) / sizeof(ports[0]);
    
    for (int i = 0; i < port_count; i++) {
        serial_port = open_serial_port(ports[i]);
        if (serial_port >= 0) {
            printf("[✓] Connected to Arduino on %s\n\n", ports[i]);
            return;
        }
    }
    
    printf("[!] Warning: Could not connect to Arduino. Running in simulation mode.\n\n");
}

void send_to_arduino(const char *message) {
    if (serial_port >= 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s\n", message);
        write(serial_port, buffer, strlen(buffer));
        sleep(1);
    }
}

int read_from_arduino(char *buffer, int size, int timeout_seconds) {
    if (serial_port < 0) return 0;
    
    time_t start = time(NULL);
    while (time(NULL) - start < timeout_seconds) {
        int bytes = read(serial_port, buffer, size - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            return bytes;
        }
        usleep(50000);
    }
    return 0;
}

void test_arduino_connection(void) {
    printf("\n[*] Testing Arduino Connection...\n");
    
    if (serial_port < 0) {
        printf("[!] Arduino not connected.\n");
        return;
    }
    
    printf("[✓] Arduino is connected on file descriptor: %d\n", serial_port);
    printf("[*] Connection test complete.\n\n");
}
