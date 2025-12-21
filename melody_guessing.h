#ifndef MELODY_GUESSING_H
# define MELODY_GUESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define LILA      "\033[38;5;141m"
#define PINK      "\033[38;5;206m"
#define GRAY      "\033[38;5;244m"
#define BOLD      "\033[1m"
#define RESET   "\033[0m"

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
    .melody_duration = 10000,
    .difficulty_level = 1
};

// functions i need
void load_song_database(void);
Song select_random_song(void);
void send_to_arduino(const char *message);
int read_from_arduino(char *buffer, int size, int timeout_seconds);
int serial_port;

// for the menu
void display_main_menu(void);
void display_game_menu(void);

// logic
void start_new_game(void);
void play_round(int round);
void get_player_responses(RoundResult *result);
void parse_arduino_response(const char *buffer, RoundResult *result, int *p1_recv, int *p2_recv);
void display_round_results(RoundResult *result, int round);
void display_final_results(void);
void process_round_data(RoundResult *result);

// settings
void change_difficulty(void);
void view_settings(void);
void reset_game(void);
void display_scoreboard(void);

# endif