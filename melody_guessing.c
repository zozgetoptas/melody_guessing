#include "melody_guessing.h"

#define DEFAULT_ROUND_TIME_MS 15000

static const char* get_melody_for_song(int song_id);
static int load_melody_database(void);

GameState game_state = {
    .current_round = 0,
    .total_rounds = 3,
    .player1_score = 0,
    .player2_score = 0,
    .melody_duration = 10000,
    .difficulty_level = 1
};

// Player names (entered by admin)
static char player1_name[32] = "Player 1";
static char player2_name[32] = "Player 2";

// UI Theme
static const char* current_primary_color = PINK;
static const char* current_secondary_color = LILA;
static int current_theme = 1; // 1=Pink/Purple, 2=Cyan/Blue, 3=Green/Yellow

// Makro for easy color switching
#define P current_primary_color
#define S current_secondary_color

#ifdef _WIN32
SerialPortHandle serial_port = INVALID_HANDLE_VALUE;
#else
SerialPortHandle serial_port = -1;
#endif

static int compute_time_points(int time_ms)
{
    if (time_ms < 0)
        return 0;
    if (time_ms >= DEFAULT_ROUND_TIME_MS)
        return 0;

    int points = (DEFAULT_ROUND_TIME_MS - time_ms) * 100 / DEFAULT_ROUND_TIME_MS;
    if (points < 0)
        points = 0;
    return points;
}

static void sleep_ms(unsigned int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep((useconds_t)ms * 1000);
#endif
}

static int serial_open_default(void)
{
    const char *port_env = getenv("ARDUINO_PORT");
    const char *port = (port_env != NULL && port_env[0] != '\0') ? port_env : "COM5";

#ifdef _WIN32
    char device_path[64];
    snprintf(device_path, sizeof(device_path), "\\\\.\\\\%s", port);

    serial_port = CreateFileA(
        device_path,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (serial_port == INVALID_HANDLE_VALUE)
    {
        printf(RED "[!] Error: Could not open serial port %s (set ARDUINO_PORT env var).\n" RESET, port);
        return -1;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(serial_port, &dcb))
    {
        printf(RED "[!] Error: GetCommState failed.\n" RESET);
        CloseHandle(serial_port);
        serial_port = INVALID_HANDLE_VALUE;
        return -1;
    }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(serial_port, &dcb))
    {
        printf(RED "[!] Error: SetCommState failed.\n" RESET);
        CloseHandle(serial_port);
        serial_port = INVALID_HANDLE_VALUE;
        return -1;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 200;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 200;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(serial_port, &timeouts);

    PurgeComm(serial_port, PURGE_RXCLEAR | PURGE_TXCLEAR);

    printf(GREEN "[✓] Connected to Arduino on %s (9600 baud).\n" RESET, port);
    return 0;
#else
    (void)port;
    printf(YELLOW "[!] Warning: Serial open not implemented for this platform in Production_Code build.\n" RESET);
    return -1;
#endif
}

void send_to_arduino(const char *message)
{
#ifdef _WIN32
    if (serial_port == INVALID_HANDLE_VALUE)
        return;

    // DEBUG: Show what we're sending
    printf(CYAN "[TX->] %s\n" RESET, message);

    size_t len = strlen(message);
    int needs_newline = (len == 0 || message[len - 1] != '\n');

    const size_t chunk_size = 256;
    size_t offset = 0;
    while (offset < len)
    {
        DWORD written = 0;
        size_t to_write = len - offset;
        if (to_write > chunk_size)
            to_write = chunk_size;
        WriteFile(serial_port, message + offset, (DWORD)to_write, &written, NULL);
        offset += to_write;
    }

    if (needs_newline)
    {
        DWORD written = 0;
        const char nl = '\n';
        WriteFile(serial_port, &nl, 1, &written, NULL);
    }
#else
    (void)message;
#endif
}

int read_from_arduino(char *buffer, int size, int timeout_seconds)
{
#ifdef _WIN32
    if (serial_port == INVALID_HANDLE_VALUE)
        return 0;

    if (size <= 1)
        return 0;

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = (DWORD)(timeout_seconds * 1000);
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 200;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(serial_port, &timeouts);

    DWORD read_bytes = 0;
    if (!ReadFile(serial_port, buffer, (DWORD)(size - 1), &read_bytes, NULL))
        return 0;

    buffer[read_bytes] = '\0';
    return (int)read_bytes;
#else
    (void)buffer;
    (void)size;
    (void)timeout_seconds;
    return 0;
#endif
}

void display_main_menu(void)
{
    printf("\e[1;1H\e[2J\n\n");

    printf("%s%s", P, BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n%s", RESET);

    printf("\n");
    printf("%s%s", P, BOLD);
    printf("                                                  ██████╗  █████╗ ████████╗████████╗██╗     ███████╗\n");
    printf("                                                  ██╔══██╗██╔══██╗╚══██╔══╝╚══██╔══╝██║     ██╔════╝\n");
    printf("                                                  ██████╔╝███████║   ██║      ██║   ██║     █████╗  \n");
    printf("                                                  ██╔══██╗██╔══██║   ██║      ██║   ██║     ██╔══╝  \n");
    printf("                                                  ██████╔╝██║  ██║   ██║      ██║   ███████╗███████╗\n");
    printf("                                                  ╚══════╝╚═╝  ╚═╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝\n%s", RESET);

    printf("\n                                                              %s%s« ADMIN CONTROL PANEL »%s\n\n", P, BOLD, RESET);

    printf("%s                                                  ╔══════════════════════════════════════════╗\n", S);
    printf("%s                                                  ║ %s%s        - CONTROL INTERFACE -           %s ║\n", S, P, BOLD, S);
    printf("%s                                                  ╠══════════════════════════════════════════╣\n", S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %s◈%s [1] %sINITIATE NEW SESSION             %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [2] %sSYSTEM SETTINGS                  %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [3] %sGLOBAL RANKINGS                  %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [4] %sFACTORY RESET                    %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [5] %sTERMINATE                        %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ╚══════════════════════════════════════════╝\n%s", S, RESET);

    printf("\n\n                                                  %s» %sACCESS CODE: %s", P, S, RESET);
}

void display_game_menu(void)
{
    printf("\e[1;1H\e[2J\n\n");
    printf("%s%s", P, BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n%s", RESET);

    printf("%s                                                  ╔══════════════════════════════════════════╗\n", S);
    printf("%s                                                  ║ %s%s        - GAME CONTROL MENU -           %s ║\n", S, P, BOLD, S);
    printf("%s                                                  ╠══════════════════════════════════════════╣\n", S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %sRound: %d / %d%s                            ║\n", S, RESET, game_state.current_round, game_state.total_rounds, S);
    printf("%s                                                  ║  %sPlayer 1: %3d pts  │  Player 2: %3d pts%s ║\n", S, RESET, game_state.player1_score, game_state.player2_score, S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %s◈%s [1] %sSTART NEXT ROUND                 %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [2] %sVIEW SCOREBOARD                  %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [3] %sRESET GAME                       %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [4] %sBACK TO MAIN MENU                %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ╚══════════════════════════════════════════╝\n%s", S, RESET);
    printf("\n                                                  %s» %sCHOICE: %s", P, S, RESET);
}

void display_final_results(void)
{
    printf("\e[1;1H\e[2J\n\n");
    printf("%s%s", P, BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n%s", RESET);

    printf("%s                                                  ╔══════════════════════════════════════════╗\n", S);
    printf("%s                                                  ║ %s%s           GAME FINISHED!          %s ║\n", S, P, BOLD, S);
    printf("%s                                                  ╠══════════════════════════════════════════╣\n", S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %s Final Scores:%s                    ║\n", S, RESET, S);
    printf("%s                                                  ║  %s%-12s: %d points%s               ║\n", S, RESET, player1_name, game_state.player1_score, S);
    printf("%s                                                  ║  %s%-12s: %d points%s               ║\n", S, RESET, player2_name, game_state.player2_score, S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %sWINNER: ", S, P);
    if (game_state.player1_score > game_state.player2_score)
        printf("%-20s%s        ║\n", player1_name, S);
    else if (game_state.player2_score > game_state.player1_score)
        printf("%-20s%s        ║\n", player2_name, S);
    else
        printf("TIE!                %s        ║\n", S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ╚══════════════════════════════════════════╝\n%s", S, RESET);
}

 void change_difficulty(void)
 {
     printf("\nSelect difficulty:\n");
     printf("  1) EASY (15 seconds)\n");
     printf("  2) MEDIUM (10 seconds)\n");
     printf("  3) HARD (5 seconds)\n");
     printf("Choice: ");

     int difficulty;
     if (scanf("%d", &difficulty) != 1)
     {
         getchar();
         printf("[!] Invalid input.\n");
         return;
     }
     getchar();

     if (difficulty == 1)
     {
         game_state.difficulty_level = 1;
         game_state.melody_duration = 15000;
     }
     else if (difficulty == 2)
     {
         game_state.difficulty_level = 2;
         game_state.melody_duration = 10000;
     }
     else if (difficulty == 3)
     {
         game_state.difficulty_level = 3;
         game_state.melody_duration = 5000;
     }
     else
     {
         printf("[!] Invalid difficulty.\n");
     }
 }

 void view_settings(void)
 {
     while (1)
     {
         printf("\n%s╔════════════════════════════════════════╗\n", S);
         printf("║%s%s    SYSTEM SETTINGS MENU                %s║\n", P, BOLD, S);
         printf("╠════════════════════════════════════════╣\n");
         printf("║  %s[1] System Information                %s║\n", RESET, S);
         printf("║  %s[2] UI/Display Settings               %s║\n", RESET, S);
         printf("║  %s[3] About / Help                      %s║\n", RESET, S);
         printf("║  %s[4] Back to Main Menu                 %s║\n", RESET, S);
         printf("╚════════════════════════════════════════╝\n%s", RESET);
         printf("\n» Choice: ");
         
         int choice;
         if (scanf("%d", &choice) != 1)
         {
             getchar();
             continue;
         }
         getchar();
         
         if (choice == 1)
         {
             printf("\n%s=== SYSTEM INFORMATION ===%s\n", GREEN, RESET);
             printf("Game: Melody Guessing Battle\n");
             printf("Total Songs in Database: 43\n");
             printf("Total Categories: 6\n");
             printf("Database Files: songs.txt, melodies.txt\n");
             printf("Scores File: highscores.txt\n");
             printf("\n%s» Press ENTER to continue...%s", P, RESET);
             getchar();
         }
         else if (choice == 2)
         {
             printf("\n%s=== UI/DISPLAY SETTINGS ===%s\n", GREEN, RESET);
             printf("Current Theme: ");
             if (current_theme == 1) printf("Pink/Purple\n");
             else if (current_theme == 2) printf("Cyan/Blue\n");
             else if (current_theme == 3) printf("Green/Yellow\n");
             printf("Text Formatting: ASCII Art Enabled\n");
             printf("Menu Style: Centered Box Layout\n");
             printf("\n%s[1] Pink/Purple Theme\n", CYAN);
             printf("[2] Cyan/Blue Theme\n");
             printf("[3] Green/Yellow Theme\n");
             printf("[4] Back\n%s", RESET);
             printf("\n» Select Theme: ");
             
             int theme_choice;
             if (scanf("%d", &theme_choice) != 1)
             {
                 getchar();
                 continue;
             }
             getchar();
             
             if (theme_choice == 1 || theme_choice == 2 || theme_choice == 3)
             {
                 current_theme = theme_choice;
                 if (theme_choice == 1)
                 {
                     current_primary_color = PINK;
                     current_secondary_color = LILA;
                 }
                 else if (theme_choice == 2)
                 {
                     current_primary_color = CYAN;
                     current_secondary_color = BLUE;
                 }
                 else if (theme_choice == 3)
                 {
                     current_primary_color = GREEN;
                     current_secondary_color = YELLOW;
                 }
                 printf("%s\n[✓] Theme changed successfully!\n%s", GREEN, RESET);
                 printf("%s» Press ENTER to continue...%s", P, RESET);
                 getchar();
             }
         }
         else if (choice == 3)
         {
             printf("\n" GREEN "=== ABOUT / HELP ===" RESET "\n");
             printf("Version: 1.0\n");
             printf("Purpose: Two-player music guessing game\n");
             printf("Controls: Menu-driven (Enter numbers 1-5)\n");
             printf("\nFeatures:\n");
             printf("  • 43 songs across 6 categories\n");
             printf("  • 3 difficulty levels\n");
             printf("  • Global ranking system\n");
             printf("  • Arduino hardware integration\n");
             printf("\n" PINK "» Press ENTER to continue..." RESET);
             getchar();
         }
         else if (choice == 4)
         {
             break;
         }
     }
 }

 void get_player_responses(RoundResult *result)
 {
     char buffer[256] = {0};
     int p1_received = 0, p2_received = 0;
     int timeout = 30;
     time_t start_time = time(NULL);

     printf("[LISTENING] Waiting for player inputs...\n");

     while (time(NULL) - start_time < timeout && (!p1_received || !p2_received))
     {
         memset(buffer, 0, sizeof(buffer));
         int bytes = read_from_arduino(buffer, (int)sizeof(buffer), 1);

         if (bytes > 0)
             parse_arduino_response(buffer, result, &p1_received, &p2_received);

         sleep_ms(50);
     }
 }

 void parse_arduino_response(const char *buffer, RoundResult *result, int *p1_recv, int *p2_recv)
 {
     int p1_ans = -1, p2_ans = -1;
     int t1 = -1, t2 = -1;

     if (
         sscanf(buffer, "P1=%d,T1=%d,P2=%d,T2=%d", &p1_ans, &t1, &p2_ans, &t2) == 4 ||
         sscanf(buffer, "P1:%d,T1:%d,P2:%d,T2:%d", &p1_ans, &t1, &p2_ans, &t2) == 4 ||
         sscanf(buffer, "P1:%d,T=%d;P2:%d,T=%d", &p1_ans, &t1, &p2_ans, &t2) == 4 ||
         sscanf(buffer, "P1:%d,%d;P2:%d,%d", &p1_ans, &t1, &p2_ans, &t2) == 4)
     {
         result->player1_guess = p1_ans;
         result->player2_guess = p2_ans;
         result->player1_time_ms = t1;
         result->player2_time_ms = t2;
         *p1_recv = 1;
         *p2_recv = 1;
         return;
     }

     if (strstr(buffer, "WINNER:P1") != NULL)
     {
         result->player1_guess = 1;
         result->player2_guess = 0;
         *p1_recv = 1;
         *p2_recv = 1;
         return;
     }

     if (strstr(buffer, "WINNER:P2") != NULL)
     {
         result->player1_guess = 0;
         result->player2_guess = 1;
         *p1_recv = 1;
         *p2_recv = 1;
         return;
     }
 }

 void process_round_data(RoundResult *result)
 {
     result->player1_points = 0;
     result->player2_points = 0;

     int p1_correct = (result->player1_guess == result->correct_answer);
     int p2_correct = (result->player2_guess == result->correct_answer);

     if (p1_correct)
         result->player1_points = compute_time_points(result->player1_time_ms);

     if (p2_correct)
         result->player2_points = compute_time_points(result->player2_time_ms);

     game_state.player1_score += result->player1_points;
     game_state.player2_score += result->player2_points;

     {
         char msg[64];
         snprintf(msg, sizeof(msg), "RESULT:P1=%s,P2=%s", p1_correct ? "OK" : "BAD", p2_correct ? "OK" : "BAD");
         send_to_arduino(msg);
     }
 }

 void display_round_results(RoundResult *result, int round)
 {
     printf("\n[ROUND %d RESULTS]\n", round);
     printf("Correct option: %d\n", result->correct_answer);
     printf("P1: guess=%d time=%dms points=+%d\n", result->player1_guess, result->player1_time_ms, result->player1_points);
     printf("P2: guess=%d time=%dms points=+%d\n", result->player2_guess, result->player2_time_ms, result->player2_points);
     printf("TOTAL => P1=%d | P2=%d\n\n", game_state.player1_score, game_state.player2_score);
 }

 void play_round(int round)
 {
     printf("\n[ROUND %d]\n", round);

     RoundResult result = {
         .player1_guess = -1,
         .player2_guess = -1,
         .correct_answer = -1,
         .player1_time_ms = -1,
         .player2_time_ms = -1,
         .player1_points = 0,
         .player2_points = 0,
         .song = select_random_song(),
         .other_song = {0}
     };

     result.other_song = select_random_song();
     for (int i = 0; i < 10 && result.other_song.id == result.song.id; i++)
         result.other_song = select_random_song();

     result.correct_answer = (rand() % 2) + 1;

     printf("Options:\n");
     printf("  1) %s - %s\n", result.song.song_name, result.song.artist);
     printf("  2) %s - %s\n", result.other_song.song_name, result.other_song.artist);
     printf("Melody duration: %d ms\n", game_state.melody_duration);

     {
         char cmd[64];
         snprintf(cmd, sizeof(cmd), "DURATION:%d", game_state.melody_duration);
         send_to_arduino(cmd);
     }

     {
         const Song *song_to_play = (result.correct_answer == 1) ? &result.song : &result.other_song;
         const char *melody = get_melody_for_song(song_to_play->id);
         if (melody != NULL && melody[0] != '\0')
         {
             size_t msg_len = strlen(melody) + strlen("MELODY:");
             char *msg = (char*)malloc(msg_len + 1);
             if (msg != NULL)
             {
                 snprintf(msg, msg_len + 1, "MELODY:%s", melody);
                 send_to_arduino(msg);
                 free(msg);
             }
         }
     }

     send_to_arduino("START");

     {
         char cmd[64];
         snprintf(cmd, sizeof(cmd), "ROUND_TIME:%d", DEFAULT_ROUND_TIME_MS);
         send_to_arduino(cmd);
     }

     get_player_responses(&result);
     process_round_data(&result);
     display_round_results(&result, round);
 }

/**
 * =============================================================================
 * DATA MANAGEMENT MODULE (Task 2)
 * Arduino Melody Guessing System 
 * =============================================================================
 * Bu dosya Task 1 (admin_console.c, main.c) ile uyumlu çalışır.
 * Şarkı veritabanı ve skor yönetimini sağlar.
 * 
 * Şarkılar: github.com/robsoncouto/arduino-songs reposu ile uyumlu
 * =============================================================================
 */

// =============================================================================
// GLOBAL DEĞİŞKENLER
// =============================================================================

#define MAX_SONGS 100
#define MAX_SCORES 50
#define SONGS_FILE "songs.txt"
#define SCORES_FILE "highscores.txt"

#define MELODIES_FILE "melodies.txt"
#define MAX_MELODIES 128
#define MAX_MELODY_STR 8192

typedef struct {
    int id;
    char melody[MAX_MELODY_STR];
} MelodyEntry;

static MelodyEntry melody_db[MAX_MELODIES];
static int melody_count = 0;

// Şarkı yapısı (Arduino repo ile uyumlu)
typedef struct {
    int id;
    char song_name[50];
    char artist[50];
    char category[32];          // Film, Oyun, Klasik, Pop, Dizi
    char arduino_file[50];      // Arduino repo klasör adı (örn: "starwars")
} SongData;

// Skor kaydı
typedef struct {
    char player_name[32];
    int score;
    int wins;
    int games_played;
    time_t timestamp;
} HighScore;

// Veritabanları
static SongData song_database[MAX_SONGS];
static int song_count = 0;

static HighScore score_board[MAX_SCORES];
static int score_count = 0;

// Kategori listesi
static const char* categories[] = {
    "Film",
    "Oyun", 
    "Klasik",
    "Pop",
    "Dizi",
    "Special"
};
static const int category_count = 6;

// Seçili kategori (-1 = hepsi)
static int selected_category = -1;





 static const char* get_melody_for_song(int song_id)
 {
     for (int i = 0; i < melody_count; i++)
     {
         if (melody_db[i].id == song_id)
             return melody_db[i].melody;
     }
     return "";
 }

static int load_melody_database(void)
{
    FILE *file = fopen(MELODIES_FILE, "r");
    if (file == NULL)
    {
        printf(YELLOW "[!] Warning: %s not found.\n" RESET, MELODIES_FILE);
        return -1;
    }

    melody_count = 0;
    char line[MAX_MELODY_STR + 64];

    while (fgets(line, sizeof(line), file) != NULL && melody_count < MAX_MELODIES)
    {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        line[strcspn(line, "\r\n")] = '\0';

        int id = -1;
        char *melody_pos = strstr(line, "MELODY:");
        if (melody_pos == NULL)
            continue;

        if (sscanf(line, "%d", &id) != 1)
            continue;

        melody_pos += strlen("MELODY:");
        while (*melody_pos == ' ')
            melody_pos++;

        melody_db[melody_count].id = id;
        strncpy(melody_db[melody_count].melody, melody_pos, sizeof(melody_db[melody_count].melody) - 1);
        melody_db[melody_count].melody[sizeof(melody_db[melody_count].melody) - 1] = '\0';
        melody_count++;
    }

    fclose(file);
    printf(GREEN "[✓] Loaded %d melodies from %s.\n" RESET, melody_count, MELODIES_FILE);
    return 0;
}



// =============================================================================
// ŞARKI VERİTABANI FONKSİYONLARI
// =============================================================================

/**
 * Şarkı veritabanını dosyadan yükler
 * Dosya formatı: ID|SongName|Artist|Category|ArduinoFile
 */
void load_song_database(void)
{
    FILE *file = fopen(SONGS_FILE, "r");
    if (file == NULL)
    {
        printf(YELLOW "[!] Warning: %s not found.\n" RESET, SONGS_FILE);
        return;
    }

    song_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && song_count < MAX_SONGS)
    {
        // Yorum ve boş satırları atla
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        // Satır sonu karakterini temizle
        line[strcspn(line, "\r\n")] = '\0';

        SongData *song = &song_database[song_count];

        int parsed = sscanf(line, "%d|%49[^|]|%49[^|]|%31[^|]|%49[^|\n]",
                            &song->id,
                            song->song_name,
                            song->artist,
                            song->category,
                            song->arduino_file);

        if (parsed >= 5)
        {
            song_count++;
        }
    }

    fclose(file);
    printf(GREEN "[✓] Loaded %d songs from database.\n" RESET, song_count);
}

/**
 * Kategoriye göre rastgele şarkı seçer
 */
Song select_random_song(void)
{
    Song result = {0};

    if (song_count == 0)
    {
        printf(RED "[!] Error: No songs in database!\n" RESET);
        return result;
    }

    // Seçili kategoriye göre filtrele
    int valid_indices[MAX_SONGS];
    int valid_count = 0;

    for (int i = 0; i < song_count; i++)
    {
        if (selected_category == -1)
        {
            // Tüm şarkılar
            valid_indices[valid_count++] = i;
        }
        else if (selected_category >= 0 && selected_category < category_count)
        {
            // Kategori eşleşmesi
            if (strcmp(song_database[i].category, categories[selected_category]) == 0)
            {
                valid_indices[valid_count++] = i;
            }
        }
    }

    if (valid_count == 0)
    {
        printf(YELLOW "[!] No songs found in selected category. Using all songs.\n" RESET);
        for (int i = 0; i < song_count; i++)
            valid_indices[valid_count++] = i;
    }

    // Rastgele seç
    int random_idx = valid_indices[rand() % valid_count];
    SongData *selected = &song_database[random_idx];

    // Task 1'in Song struct'ına kopyala
    result.id = selected->id;
    strncpy(result.song_name, selected->song_name, sizeof(result.song_name) - 1);
    strncpy(result.artist, selected->artist, sizeof(result.artist) - 1);
    result.melody_duration = 5000; // Varsayılan

    return result;
}

/**
 * Şarkının Arduino dosya adını döndürür
 * Arduino'ya gönderilecek komut: "PLAY:starwars" gibi
 */
const char* get_arduino_filename(int song_id)
{
    for (int i = 0; i < song_count; i++)
    {
        if (song_database[i].id == song_id)
        {
            return song_database[i].arduino_file;
        }
    }
    return "";
}

/**
 * Şarkının kategorisini döndürür
 */
const char* get_song_category(int song_id)
{
    for (int i = 0; i < song_count; i++)
    {
        if (song_database[i].id == song_id)
        {
            return song_database[i].category;
        }
    }
    return "Unknown";
}

/**
 * Toplam şarkı sayısını döndürür
 */
int get_total_song_count(void)
{
    return song_count;
}

// =============================================================================
// KATEGORİ FONKSİYONLARI
// =============================================================================

/**
 * Kategori seçim menüsünü gösterir ve seçimi alır
 */
int display_category_menu(void)
{
    printf("\n");
    printf("%s                                                  ╔══════════════════════════════════════════╗\n", S);
    printf("%s                                                  ║ %s%s         - SELECT CATEGORY -            %s ║\n", S, P, BOLD, S);
    printf("%s                                                  ╠══════════════════════════════════════════╣\n", S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ║  %s◈%s [1] %sFilm Muzikleri                   %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [2] %sOyun Muzikleri                   %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [3] %sKlasik Muzik                     %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [4] %sPop                              %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s◈%s [5] %sDizi Muzikleri                   %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║  %s★%s [6] %sSpecial Selection (Best)         %s ║\n", S, P, S, YELLOW, S);
    printf("%s                                                  ║  %s◈%s [7] %sTum Kategoriler (Karisik)        %s ║\n", S, P, S, RESET, S);
    printf("%s                                                  ║                                          ║\n", S);
    printf("%s                                                  ╚══════════════════════════════════════════╝\n%s", S, RESET);
    printf("\n                                                  %s» %sSELECT: %s", P, S, RESET);

    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 7)
    {
        getchar();
        return -1;
    }
    getchar();

    if (choice == 7)
        selected_category = -1;  // All categories
    else
        selected_category = choice - 1;  // 0-indexed (6 = Special)

    return choice;
}

/**
 * Seçili kategorideki şarkı sayısını döndürür
 */
int get_category_song_count(int category_index)
{
    if (category_index < 0)
        return song_count;

    int count = 0;
    for (int i = 0; i < song_count; i++)
    {
        if (strcmp(song_database[i].category, categories[category_index]) == 0)
            count++;
    }
    return count;
}

// =============================================================================
// SKOR TABLOSU FONKSİYONLARI
// =============================================================================

/**
 * Skor tablosunu dosyadan yükler
 */
void load_scores(void)
{
    FILE *file = fopen(SCORES_FILE, "r");
    if (file == NULL)
        return;

    score_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL && score_count < MAX_SCORES)
    {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        line[strcspn(line, "\r\n")] = '\0';

        HighScore *hs = &score_board[score_count];
        long ts;

        if (sscanf(line, "%31[^|]|%d|%d|%d|%ld",
                   hs->player_name, &hs->score, &hs->wins,
                   &hs->games_played, &ts) >= 4)
        {
            hs->timestamp = (time_t)ts;
            score_count++;
        }
    }

    fclose(file);
}

/**
 * Skor tablosunu dosyaya kaydeder
 */
void save_scores(void)
{
    FILE *file = fopen(SCORES_FILE, "w");
    if (file == NULL)
        return;

    fprintf(file, "# High Scores - Melody Guessing Battle\n");
    fprintf(file, "# Format: Name|Score|Wins|GamesPlayed\n\n");

    for (int i = 0; i < score_count; i++)
    {
        fprintf(file, "%s|%d|%d|%d\n",
                score_board[i].player_name,
                score_board[i].score,
                score_board[i].wins,
                score_board[i].games_played);
    }

    fclose(file);
}

/**
 * Yeni skor ekler
 */
void add_score(const char *name, int score, int won)
{
    // Mevcut oyuncu var mı kontrol et
    for (int i = 0; i < score_count; i++)
    {
        if (strcmp(score_board[i].player_name, name) == 0)
        {
            // Güncelle
            score_board[i].score += score;
            score_board[i].games_played++;
            if (won)
                score_board[i].wins++;
            score_board[i].timestamp = time(NULL);
            save_scores();
            return;
        }
    }

    // Yeni oyuncu ekle
    if (score_count < MAX_SCORES)
    {
        HighScore *hs = &score_board[score_count];
        strncpy(hs->player_name, name, sizeof(hs->player_name) - 1);
        hs->score = score;
        hs->wins = won ? 1 : 0;
        hs->games_played = 1;
        hs->timestamp = time(NULL);
        score_count++;
        save_scores();
    }
}

/**
 * Skor tablosunu gösterir
 */
void display_scoreboard(void)
{
    load_scores();

    printf("\n");
    printf("%s                                                  ╔══════════════════════════════════════════╗\n", S);
    printf("%s                                                  ║ %s%s         GLOBAL RANKINGS                %s ║\n", S, P, BOLD, S);
    printf("%s                                                  ╠══════════════════════════════════════════╣\n", S);

    if (score_count == 0)
    {
        printf("%s                                                  ║                                          ║\n", S);
        printf("%s                                                  ║  %s       No scores recorded yet.         %s ║\n", S, RESET, S);
        printf("%s                                                  ║                                          ║\n", S);
    }
    else
    {
        // Skorları sırala (bubble sort)
        for (int i = 0; i < score_count - 1; i++)
        {
            for (int j = 0; j < score_count - i - 1; j++)
            {
                if (score_board[j].score < score_board[j + 1].score)
                {
                    HighScore temp = score_board[j];
                    score_board[j] = score_board[j + 1];
                    score_board[j + 1] = temp;
                }
            }
        }

        printf("%s                                                  ║  %s#   Player          Score   Wins       %s ║\n", S, RESET, S);
        printf("%s                                                  ║  %s─────────────────────────────────────   %s ║\n", S, RESET, S);

        int show = (score_count < 10) ? score_count : 10;
        for (int i = 0; i < show; i++)
        {
            printf("%s                                                  ║  %s%-2d  %-15s %5d   %3d        %s ║\n", S, RESET,
                   i + 1,
                   score_board[i].player_name,
                   score_board[i].score,
                   score_board[i].wins,
                   S);
        }
        printf("%s                                                  ║                                          ║\n", S);
    }

    printf("%s                                                  ╚══════════════════════════════════════════╝\n%s", S, RESET);
    printf("\n                                                  %s» %sPress ENTER to continue...%s", P, S, RESET);
    getchar();
}

// =============================================================================
// OYUN YÖNETİM FONKSİYONLARI
// =============================================================================

/**
 * Oyunu sıfırlar
 */
void reset_game(void)
{
    game_state.current_round = 0;
    game_state.player1_score = 0;
    game_state.player2_score = 0;
    selected_category = -1;
    
    // Delete highscores file and clear RAM
    remove("highscores.txt");
    score_count = 0;
    
    printf(GREEN "[✓] Game state reset. Highscores cleared.\n" RESET);
    printf(PINK "» " LILA "Press ENTER to continue..." RESET);
    getchar();
}

/**
 * Saves game results
 */
void save_game_results(void)
{
    int p1_won = (game_state.player1_score > game_state.player2_score) ? 1 : 0;
    int p2_won = (game_state.player2_score > game_state.player1_score) ? 1 : 0;

    add_score(player1_name, game_state.player1_score, p1_won);
    add_score(player2_name, game_state.player2_score, p2_won);
}

// =============================================================================
// ARDUINO İLETİŞİM YARDIMCI FONKSİYONLARI
// =============================================================================

/**
 * Arduino'ya şarkı çalma komutu gönderir
 * Format: "PLAY:starwars" veya "PLAY:tetris"
 * Arduino robsoncouto/arduino-songs reposundaki melodileri çalacak
 */
void send_song_to_arduino(int song_id)
{
    const char *filename = get_arduino_filename(song_id);
    if (strlen(filename) > 0)
    {
        char command[100];
        snprintf(command, sizeof(command), "PLAY:%s", filename);
        send_to_arduino(command);
        printf(CYAN "[→] Sent to Arduino: %s\n" RESET, command);
    }
}

/**
 * Arduino'ya zorluk süresini gönderir
 */
void send_duration_to_arduino(int duration_ms)
{
    char command[50];
    snprintf(command, sizeof(command), "DURATION:%d", duration_ms);
    send_to_arduino(command);
}

void start_new_game(void)
{
    printf("\n[*] Starting New Game...\n\n");
    
    // Get player names
    printf("Enter Player 1 name: ");
    fgets(player1_name, sizeof(player1_name), stdin);
    player1_name[strcspn(player1_name, "\r\n")] = '\0';
    if (strlen(player1_name) == 0) strcpy(player1_name, "Player 1");
    
    printf("Enter Player 2 name: ");
    fgets(player2_name, sizeof(player2_name), stdin);
    player2_name[strcspn(player2_name, "\r\n")] = '\0';
    if (strlen(player2_name) == 0) strcpy(player2_name, "Player 2");
    
    // Select number of rounds
    printf("\nHow many rounds? (1-10): ");
    int rounds;
    if (scanf("%d", &rounds) != 1 || rounds < 1 || rounds > 10)
    {
        getchar();
        printf("[!] Invalid input. Using default 3 rounds.\n");
        rounds = 3;
    }
    getchar();
    game_state.total_rounds = rounds;
    
    printf("\n");
    
    // Select category
    display_category_menu();

    // Select difficulty
    printf("\n[*] Select Difficulty:\n");
    change_difficulty();

    reset_game();
    game_state.current_round = 1;
    game_state.total_rounds = rounds;  // Restore after reset
    printf("[✓] Game initialized: %s vs %s (%d rounds)\n\n", player1_name, player2_name, rounds);

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

    display_final_results();
    save_game_results();
    printf(GREEN "[✓] Scores saved to highscores.txt\n" RESET);
    
    printf("\nPress ENTER to continue...");
    getchar();
    
    reset_game();
}

int main(void)
{
    srand(time(NULL));
    
    printf("\n");

    // Skip serial port for demo mode
    // if (serial_open_default() != 0)
    //     return 1;

    load_song_database();

    load_melody_database();
    
    while (1)
    {
        display_main_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            getchar();  // clear invalid input
            printf("[!] Invalid input. Please enter a number (1-6).\n");
            continue;
        }
        getchar();  // clear newline
        
        switch (choice)
        {
            case 1: 
                start_new_game();           
                break;
            case 2: 
                view_settings();            
                break;
            case 3:
                display_scoreboard();       
                break;
            case 4:
                reset_game();               
                break;
            case 5:
                printf("\n[*] Exiting Admin Console. Goodbye!\n\n");
                return 0;
            default:
                printf("[!] Invalid choice (1-5).\n");
        }
    }
    return 0;
}
