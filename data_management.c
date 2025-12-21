// CSE 101 - MELODY GUESSING SYSTEM
// DATA MANAGEMENT MODULE (Task 2)
// Functions: File I/O, Structs, Data Persistence

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// =========================================================================
// DATA STRUCTURES
// =========================================================================

typedef struct {
    int id;
    char song_name[100];
    char artist[100];
    char genre[50];
    int frequency_array[100];  // For melody frequencies
    int duration_array[100];   // For note durations
    int note_count;
} Song;

typedef struct {
    char player_name[100];
    int score;
    int round_number;
    char date[20];
    int difficulty_level;
} HighScore;

typedef struct {
    int song_id;
    int attempt_count;
    int correct_count;
    int total_plays;
} SongStatistics;

// =========================================================================
// FILE PATHS
// =========================================================================

#define SONGS_DATABASE_FILE "songs_database.txt"
#define HIGHSCORES_FILE "highscores.txt"
#define STATISTICS_FILE "song_statistics.txt"
#define BACKUP_FOLDER "backups/"

// =========================================================================
// FUNCTION PROTOTYPES
// =========================================================================

// Song Database Functions
int load_songs_from_file(Song *songs);
int save_songs_to_file(Song *songs, int song_count);
void display_song_database(Song *songs, int song_count);
void add_new_song(Song *songs, int *song_count);
void delete_song(Song *songs, int *song_count, int song_id);
void search_song_by_name(Song *songs, int song_count, const char *name);

// High Score Functions
int load_highscores_from_file(HighScore *scores);
int save_highscores_to_file(HighScore *scores, int score_count);
void display_highscores(HighScore *scores, int score_count);
void add_highscore(HighScore *scores, int *score_count, const char *name, int score, int round, int difficulty);
void sort_highscores(HighScore *scores, int score_count);

// Statistics Functions
int load_statistics_from_file(SongStatistics *stats);
int save_statistics_to_file(SongStatistics *stats, int stats_count);
void update_song_statistics(SongStatistics *stats, int stats_count, int song_id, int correct);
void display_statistics(SongStatistics *stats, int stats_count);

// Utility Functions
void backup_database(const char *filename);
void validate_database(Song *songs, int song_count);
void print_menu(void);

// =========================================================================
// SONG DATABASE FUNCTIONS
// =========================================================================

int load_songs_from_file(Song *songs) {
    FILE *file = fopen(SONGS_DATABASE_FILE, "r");
    
    if (!file) {
        printf("[!] Songs database file not found. Creating default database...\n");
        // Create default songs
        Song default_songs[5] = {
            {1, "Super Mario", "Nintendo", "Game Theme", {0}, {0}, 0},
            {2, "Happy Birthday", "Traditional", "Traditional", {0}, {0}, 0},
            {3, "Jingle Bells", "James Pierpont", "Christmas", {0}, {0}, 0},
            {4, "Twinkle Twinkle", "Traditional", "Children", {0}, {0}, 0},
            {5, "Mary Had Lamb", "Traditional", "Children", {0}, {0}, 0}
        };
        
        FILE *new_file = fopen(SONGS_DATABASE_FILE, "w");
        if (!new_file) {
            printf("[!] Error: Cannot create songs database file.\n");
            return 0;
        }
        
        for (int i = 0; i < 5; i++) {
            fprintf(new_file, "%d|%s|%s|%s|%d\n", 
                default_songs[i].id, 
                default_songs[i].song_name, 
                default_songs[i].artist, 
                default_songs[i].genre,
                default_songs[i].note_count);
            songs[i] = default_songs[i];
        }
        
        fclose(new_file);
        printf("[✓] Default database created with 5 songs.\n");
        return 5;
    }
    
    int count = 0;
    char line[500];
    
    while (fgets(line, sizeof(line), file) && count < 100) {
        if (line[0] == '#' || line[0] == '\n') continue;  // Skip comments and empty lines
        
        sscanf(line, "%d|%99[^|]|%99[^|]|%49[^|]|%d",
            &songs[count].id,
            songs[count].song_name,
            songs[count].artist,
            songs[count].genre,
            &songs[count].note_count);
        
        count++;
    }
    
    fclose(file);
    printf("[✓] Loaded %d songs from database.\n", count);
    return count;
}

int save_songs_to_file(Song *songs, int song_count) {
    FILE *file = fopen(SONGS_DATABASE_FILE, "w");
    
    if (!file) {
        printf("[!] Error: Cannot open songs database file for writing.\n");
        return 0;
    }
    
    fprintf(file, "# Melody Guessing System - Song Database\n");
    fprintf(file, "# Format: ID|SongName|Artist|Genre|NoteCount\n\n");
    
    for (int i = 0; i < song_count; i++) {
        fprintf(file, "%d|%s|%s|%s|%d\n",
            songs[i].id,
            songs[i].song_name,
            songs[i].artist,
            songs[i].genre,
            songs[i].note_count);
    }
    
    fclose(file);
    printf("[✓] Saved %d songs to database.\n", song_count);
    return 1;
}

void display_song_database(Song *songs, int song_count) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    SONG DATABASE (%d)                     ║\n", song_count);
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ ID | Song Name          | Artist         | Genre            ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < song_count && i < 20; i++) {
        printf("║ %2d | %-18s | %-14s | %-16s ║\n",
            songs[i].id,
            songs[i].song_name,
            songs[i].artist,
            songs[i].genre);
    }
    
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

void add_new_song(Song *songs, int *song_count) {
    if (*song_count >= 100) {
        printf("[!] Database is full. Cannot add more songs.\n");
        return;
    }
    
    Song new_song;
    new_song.id = songs[*song_count - 1].id + 1;
    new_song.note_count = 0;
    
    printf("\n[*] Adding new song...\n");
    printf("Song name: ");
    fgets(new_song.song_name, sizeof(new_song.song_name), stdin);
    new_song.song_name[strcspn(new_song.song_name, "\n")] = 0;
    
    printf("Artist: ");
    fgets(new_song.artist, sizeof(new_song.artist), stdin);
    new_song.artist[strcspn(new_song.artist, "\n")] = 0;
    
    printf("Genre: ");
    fgets(new_song.genre, sizeof(new_song.genre), stdin);
    new_song.genre[strcspn(new_song.genre, "\n")] = 0;
    
    songs[*song_count] = new_song;
    (*song_count)++;
    
    printf("[✓] Song added successfully with ID %d.\n", new_song.id);
    save_songs_to_file(songs, *song_count);
}

void delete_song(Song *songs, int *song_count, int song_id) {
    int found_index = -1;
    
    for (int i = 0; i < *song_count; i++) {
        if (songs[i].id == song_id) {
            found_index = i;
            break;
        }
    }
    
    if (found_index == -1) {
        printf("[!] Song with ID %d not found.\n", song_id);
        return;
    }
    
    // Shift array elements
    for (int i = found_index; i < *song_count - 1; i++) {
        songs[i] = songs[i + 1];
    }
    
    (*song_count)--;
    printf("[✓] Song deleted. Database now has %d songs.\n", *song_count);
    save_songs_to_file(songs, *song_count);
}

void search_song_by_name(Song *songs, int song_count, const char *name) {
    printf("\n[*] Searching for songs containing '%s'...\n\n", name);
    
    int found_count = 0;
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                       SEARCH RESULTS                           ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < song_count; i++) {
        if (strstr(songs[i].song_name, name) != NULL || 
            strstr(songs[i].artist, name) != NULL) {
            printf("║ [ID: %d] %s - %s (%s)\n", 
                songs[i].id, songs[i].song_name, songs[i].artist, songs[i].genre);
            found_count++;
        }
    }
    
    if (found_count == 0) {
        printf("║ No songs found.\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

// =========================================================================
// HIGH SCORE FUNCTIONS
// =========================================================================

int load_highscores_from_file(HighScore *scores) {
    FILE *file = fopen(HIGHSCORES_FILE, "r");
    
    if (!file) {
        printf("[!] High scores file not found. Starting with empty list.\n");
        return 0;
    }
    
    int count = 0;
    char line[500];
    
    while (fgets(line, sizeof(line), file) && count < 100) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        sscanf(line, "%99[^|]|%d|%d|%19[^|]|%d",
            scores[count].player_name,
            &scores[count].score,
            &scores[count].round_number,
            scores[count].date,
            &scores[count].difficulty_level);
        
        count++;
    }
    
    fclose(file);
    printf("[✓] Loaded %d high scores.\n", count);
    return count;
}

int save_highscores_to_file(HighScore *scores, int score_count) {
    FILE *file = fopen(HIGHSCORES_FILE, "w");
    
    if (!file) {
        printf("[!] Error: Cannot open high scores file for writing.\n");
        return 0;
    }
    
    fprintf(file, "# Melody Guessing System - High Scores\n");
    fprintf(file, "# Format: PlayerName|Score|RoundNumber|Date|DifficultyLevel\n\n");
    
    for (int i = 0; i < score_count; i++) {
        fprintf(file, "%s|%d|%d|%s|%d\n",
            scores[i].player_name,
            scores[i].score,
            scores[i].round_number,
            scores[i].date,
            scores[i].difficulty_level);
    }
    
    fclose(file);
    return 1;
}

void display_highscores(HighScore *scores, int score_count) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    HIGH SCORES (Top %d)                  ║\n", score_count < 10 ? score_count : 10);
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ Rank | Player Name        | Score | Difficulty | Date       ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    int display_count = score_count < 10 ? score_count : 10;
    for (int i = 0; i < display_count; i++) {
        const char *difficulty = (scores[i].difficulty_level == 1) ? "Easy" : 
                                 (scores[i].difficulty_level == 2) ? "Medium" : "Hard";
        printf("║ %2d  | %-17s | %5d | %-10s | %10s ║\n",
            i + 1,
            scores[i].player_name,
            scores[i].score,
            difficulty,
            scores[i].date);
    }
    
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

void add_highscore(HighScore *scores, int *score_count, const char *name, int score, int round, int difficulty) {
    if (*score_count >= 100) {
        printf("[!] High scores list is full.\n");
        return;
    }
    
    strcpy(scores[*score_count].player_name, name);
    scores[*score_count].score = score;
    scores[*score_count].round_number = round;
    scores[*score_count].difficulty_level = difficulty;
    
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(scores[*score_count].date, 20, "%Y-%m-%d", timeinfo);
    
    (*score_count)++;
    sort_highscores(scores, *score_count);
    save_highscores_to_file(scores, *score_count);
    
    printf("[✓] High score saved!\n");
}

void sort_highscores(HighScore *scores, int score_count) {
    // Bubble sort by score (descending)
    for (int i = 0; i < score_count - 1; i++) {
        for (int j = 0; j < score_count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

// =========================================================================
// STATISTICS FUNCTIONS
// =========================================================================

int load_statistics_from_file(SongStatistics *stats) {
    FILE *file = fopen(STATISTICS_FILE, "r");
    
    if (!file) {
        printf("[!] Statistics file not found.\n");
        return 0;
    }
    
    int count = 0;
    char line[200];
    
    while (fgets(line, sizeof(line), file) && count < 100) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        sscanf(line, "%d|%d|%d|%d",
            &stats[count].song_id,
            &stats[count].attempt_count,
            &stats[count].correct_count,
            &stats[count].total_plays);
        
        count++;
    }
    
    fclose(file);
    return count;
}

int save_statistics_to_file(SongStatistics *stats, int stats_count) {
    FILE *file = fopen(STATISTICS_FILE, "w");
    
    if (!file) {
        printf("[!] Error: Cannot open statistics file for writing.\n");
        return 0;
    }
    
    fprintf(file, "# Song Statistics\n");
    fprintf(file, "# Format: SongID|AttemptCount|CorrectCount|TotalPlays\n\n");
    
    for (int i = 0; i < stats_count; i++) {
        fprintf(file, "%d|%d|%d|%d\n",
            stats[i].song_id,
            stats[i].attempt_count,
            stats[i].correct_count,
            stats[i].total_plays);
    }
    
    fclose(file);
    return 1;
}

void update_song_statistics(SongStatistics *stats, int stats_count, int song_id, int correct) {
    for (int i = 0; i < stats_count; i++) {
        if (stats[i].song_id == song_id) {
            stats[i].attempt_count++;
            stats[i].total_plays++;
            if (correct) {
                stats[i].correct_count++;
            }
            save_statistics_to_file(stats, stats_count);
            return;
        }
    }
    
    printf("[!] Song ID %d not found in statistics.\n", song_id);
}

void display_statistics(SongStatistics *stats, int stats_count) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                      SONG STATISTICS                          ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ SongID | Attempts | Correct | Total Plays | Success Rate     ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < stats_count && i < 20; i++) {
        float success_rate = (stats[i].attempt_count > 0) ? 
            ((float)stats[i].correct_count / stats[i].attempt_count) * 100 : 0;
        
        printf("║ %6d | %8d | %7d | %11d | %15.1f%% ║\n",
            stats[i].song_id,
            stats[i].attempt_count,
            stats[i].correct_count,
            stats[i].total_plays,
            success_rate);
    }
    
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

// =========================================================================
// UTILITY FUNCTIONS
// =========================================================================

void backup_database(const char *filename) {
    printf("[*] Creating backup of %s...\n", filename);
    
    char backup_path[200];
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    
    snprintf(backup_path, sizeof(backup_path), "%s%s_%04d%02d%02d_%02d%02d%02d.bak",
        BACKUP_FOLDER,
        filename,
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec);
    
    // Simple file copy
    FILE *src = fopen(filename, "r");
    FILE *dst = fopen(backup_path, "w");
    
    if (src && dst) {
        char buffer[1024];
        int bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            fwrite(buffer, 1, bytes, dst);
        }
        fclose(src);
        fclose(dst);
        printf("[✓] Backup created: %s\n", backup_path);
    } else {
        printf("[!] Error creating backup.\n");
    }
}

void validate_database(Song *songs, int song_count) {
    printf("[*] Validating song database...\n");
    
    int errors = 0;
    
    for (int i = 0; i < song_count; i++) {
        if (strlen(songs[i].song_name) == 0) {
            printf("[!] Song %d has empty name.\n", songs[i].id);
            errors++;
        }
        if (strlen(songs[i].artist) == 0) {
            printf("[!] Song %d has empty artist.\n", songs[i].id);
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("[✓] Database validation passed!\n");
    } else {
        printf("[!] Found %d validation errors.\n", errors);
    }
}

void print_menu(void) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║            DATA MANAGEMENT MENU (TEST MODE)                   ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ 1. Display Song Database                                       ║\n");
    printf("║ 2. Add New Song                                                ║\n");
    printf("║ 3. Delete Song                                                 ║\n");
    printf("║ 4. Search Song by Name                                         ║\n");
    printf("║ 5. Display High Scores                                         ║\n");
    printf("║ 6. Display Statistics                                          ║\n");
    printf("║ 7. Validate Database                                           ║\n");
    printf("║ 8. Backup Database                                             ║\n");
    printf("║ 9. Exit                                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("Choice (1-9): ");
}

// =========================================================================
// TEST MAIN (Optional - for standalone testing)
// =========================================================================

/*
int main() {
    Song songs[100];
    HighScore highscores[100];
    SongStatistics stats[100];
    
    int song_count = load_songs_from_file(songs);
    int score_count = load_highscores_from_file(highscores);
    int stats_count = load_statistics_from_file(stats);
    
    int choice;
    
    while (1) {
        print_menu();
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1:
                display_song_database(songs, song_count);
                break;
            case 2:
                add_new_song(songs, &song_count);
                break;
            case 3: {
                int id;
                printf("Enter song ID to delete: ");
                scanf("%d", &id);
                delete_song(songs, &song_count, id);
                break;
            }
            case 4: {
                char search_term[100];
                printf("Enter search term: ");
                fgets(search_term, sizeof(search_term), stdin);
                search_term[strcspn(search_term, "\n")] = 0;
                search_song_by_name(songs, song_count, search_term);
                break;
            }
            case 5:
                display_highscores(highscores, score_count);
                break;
            case 6:
                display_statistics(stats, stats_count);
                break;
            case 7:
                validate_database(songs, song_count);
                break;
            case 8:
                backup_database(SONGS_DATABASE_FILE);
                break;
            case 9:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    
    return 0;
}
*/
