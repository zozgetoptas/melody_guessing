#include "melody_guessing.h"

void display_main_menu(void)
{
    printf("\e[1;1H\e[2J\n\n");

    printf(PINK BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n" RESET);

    printf("\n");
    printf(PINK BOLD);
    printf("                                                  ██████╗  █████╗ ████████╗████████╗██╗     ███████╗\n");
    printf("                                                  ██╔══██╗██╔══██╗╚══██╔══╝╚══██╔══╝██║     ██╔════╝\n");
    printf("                                                  ██████╔╝███████║   ██║      ██║   ██║     █████╗  \n");
    printf("                                                  ██╔══██╗██╔══██║   ██║      ██║   ██║     ██╔══╝  \n");
    printf("                                                  ██████╔╝██║  ██║   ██║      ██║   ███████╗███████╗\n");
    printf("                                                  ╚══════╝╚═╝  ╚═╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝\n" RESET);

    printf("\n                                                              " PINK BOLD "« ADMIN CONTROL PANEL »" RESET "\n\n");

    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "        - CONTROL INTERFACE -           " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [1] " RESET "INITIATE NEW SESSION             " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [2] " RESET "DIFFICULTY CONFIGURATION         " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [3] " RESET "SYSTEM SETTINGS                  " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [4] " RESET "GLOBAL RANKINGS                  " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [5] " RESET "FACTORY RESET                    " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [6] " RESET "TERMINATE                        " LILA " ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);

    printf("\n\n                                                  " PINK "» " LILA "ACCESS CODE: \n" RESET);

}

void display_game_menu(void)
{
    printf("\e[1;1H\e[2J\n\n");
    printf(PINK BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n" RESET);
    
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "        - GAME CONTROL MENU -           " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "Round: %d / %d" LILA "                           ║\n", game_state.current_round, game_state.total_rounds);
    printf(LILA "                                                  ║  " RESET "Player 1: %3d pts  │  Player 2: %3d pts" LILA "   ║\n", game_state.player1_score, game_state.player2_score);
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [1] " RESET "START NEXT ROUND           " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [2] " RESET "VIEW SCOREBOARD            " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [3] " RESET "RESET GAME                 " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [4] " RESET "BACK TO MAIN MENU           " LILA " ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
    printf("\n                                                  " PINK "» " LILA "CHOICE: " RESET);
}

void start_new_game(void)
{
    printf("\n[*] Starting New Game...\n");
    printf("[*] Select Theme:\n");
    printf("    1. ...\n");
    printf("    2. ...\n");
    printf("    3. ...\n");
    printf("Choice (1-3): ");
    
    int theme;
    if (scanf("%d", &theme) != 1 || theme < 1 || theme > 3)
    {
        getchar();
        printf("[!] Invalid theme selection.\n");
        return;
    }
    getchar();
    
    reset_game();
    game_state.current_round = 1;
    printf("[✓] Theme %d selected. Game initialized.\n\n", theme);
    
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
    reset_game();
}

void display_final_results(void)
{
    printf("\e[1;1H\e[2J\n\n");
    printf(PINK BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n" RESET);
    
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "          🎉 GAME FINISHED! 🎉         " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "📊 Final Scores:" LILA "                    ║\n");
    printf(LILA "                                                  ║  " RESET "Player 1: %d points" LILA "                   ║\n", game_state.player1_score);
    printf(LILA "                                                  ║  " RESET "Player 2: %d points" LILA "                   ║\n", game_state.player2_score);
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " PINK "🏆 ");
    if (game_state.player1_score > game_state.player2_score)
        printf("WINNER: Player 1 🥇" LILA "                  ║\n");
    else if (game_state.player2_score > game_state.player1_score)
        printf("WINNER: Player 2 🥇" LILA "                  ║\n");
    else
        printf("RESULT: TIE! 🤝" LILA "                      ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
}

void play_round(int round)
{
    printf("\n  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                   🎵 ROUND %d STARTING 🎵                    ║\n", round);
    printf("  ╚══════════════════════════════════════════════════════════════╝\n\n");
    
    RoundResult result =
    {
        .player1_guess = -1,
        .player2_guess = -1,
        .correct_answer = -1,
        .song = select_random_song()
    };
    
    printf("[✓] Song selected: %s by %s\n", result.song.song_name, result.song.artist);
    printf("[*] Melody duration: %d seconds\n\n", game_state.melody_duration / 1000);

    send_to_arduino("START");
    printf("[*] Signal sent to Arduino. Waiting for responses...\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    get_player_responses(&result);
    
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    process_round_data(&result);
    display_round_results(&result, round);
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
        int bytes = read(serial_port, buffer, sizeof(buffer) - 1);
        
        if (bytes > 0)
            parse_arduino_response(buffer, result, &p1_received, &p2_received);
        
        usleep(50000);
    }
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
    printf("\e[1;1H\e[2J\n\n");
    printf(PINK BOLD);
    printf("                ███╗   ███╗███████╗██╗      ██████╗ ██████╗ ██╗   ██╗    ██████╗  ██╗   ██╗███████╗███████╗███████╗██╗███╗   ██╗ ██████╗ \n");
    printf("                ████╗ ████║██╔════╝██║     ██╔═══██╗██╔══██╗╚██╗ ██╔╝    ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝██║████╗  ██║██╔════╝ \n");
    printf("                ██╔████╔██║█████╗  ██║     ██║   ██║██║  ██║ ╚████╔╝     ██║  ███╗██║   ██║█████╗  ███████╗███████╗██║██╔██╗ ██║██║  ███╗\n");
    printf("                ██║╚██╔╝██║██╔══╝  ██║     ██║   ██║██║  ██║  ╚██╔╝      ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║██║██║╚██╗██║██║   ██║\n");
    printf("                ██║ ╚═╝ ██║███████╗███████╗╚██████╔╝██████╔╝   ██║       ╚██████╔╝╚██████╔╝███████╗███████║███████║██║██║ ╚████║╚██████╔╝\n");
    printf("                ╚═╝     ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝    ╚═╝        ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ \n" RESET);
    
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "          - ROUND %d RESULTS -          " LILA " ║\n", round);
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "🎵 %s - %s" LILA "              ║\n", result->song.song_name, result->song.artist);
    printf(LILA "                                                  ║  " RESET "✔ Correct Answer: Option %d" LILA "          ║\n", result->correct_answer);
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "Player 1: ");
    if (result->player1_guess == result->correct_answer)
        printf("✓ CORRECT! (+10 pts)" LILA "      ║\n");
    else
        printf("✗ WRONG" LILA "                    ║\n");
    printf(LILA "                                                  ║  " RESET "Player 2: ");
    if (result->player2_guess == result->correct_answer)
        printf("✓ CORRECT! (+10 pts)" LILA "      ║\n");
    else
        printf("✗ WRONG" LILA "                    ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "Score: P1: %d pts │ P2: %d pts" LILA "        ║\n", game_state.player1_score, game_state.player2_score);
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
}

void change_difficulty(void) 
{
    printf("\n");
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "    - DIFFICULTY CONFIGURATION -      " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [1] " RESET "EASY (10 seconds)            " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [2] " RESET "MEDIUM (6 seconds)          " LILA " ║\n");
    printf(LILA "                                                  ║  " PINK "◈" LILA " [3] " RESET "HARD (4 seconds)            " LILA " ║\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
    printf("\n                                                  " PINK "» " LILA "SELECT DIFFICULTY: " RESET);
    
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

void view_settings(void)
{
    const char *difficulty_name[] = {"NONE", "EASY", "MEDIUM", "HARD"};
    printf("\n");
    printf(LILA "                                                  ╔══════════════════════════════════════════╗\n");
    printf(LILA "                                                  ║ " PINK BOLD "         - SYSTEM SETTINGS -           " LILA " ║\n");
    printf(LILA "                                                  ╠══════════════════════════════════════════╣\n");
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ║  " RESET "Difficulty Level: %s" LILA "              ║\n", difficulty_name[game_state.difficulty_level]);
    printf(LILA "                                                  ║  " RESET "Melody Duration:  %d seconds" LILA "         ║\n", game_state.melody_duration / 1000);
    printf(LILA "                                                  ║  " RESET "Total Rounds:     %d" LILA "                 ║\n", game_state.total_rounds);
    printf(LILA "                                                  ║                                          ║\n");
    printf(LILA "                                                  ╚══════════════════════════════════════════╝\n" RESET);
    printf("\n");
}
