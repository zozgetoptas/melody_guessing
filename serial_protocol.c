// CSE 101 - MELODY GUESSING SYSTEM
// SERIAL PROTOCOL MODULE (Task 3)
// Functions: Arduino communication, data parsing, protocol handling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

// =========================================================================
// PROTOCOL DEFINITIONS
// =========================================================================

#define BAUD_RATE B9600
#define SERIAL_TIMEOUT 30  // seconds
#define BUFFER_SIZE 256
#define MAX_MELODY_NOTES 100

// Message types
typedef enum {
    MSG_START,           // Start round
    MSG_OPTIONS,         // Send song options
    MSG_P1_GUESS,        // Player 1 guess
    MSG_P2_GUESS,        // Player 2 guess
    MSG_RESULTS,         // Send results
    MSG_RESET,           // Reset Arduino
    MSG_TEST,            // Connection test
    MSG_ACK,             // Acknowledgment
    MSG_ERROR            // Error message
} MessageType;

// =========================================================================
// DATA STRUCTURES
// =========================================================================

typedef struct {
    int id;
    int frequency;
    int duration;
} Note;

typedef struct {
    int melody_id;
    Note notes[MAX_MELODY_NOTES];
    int note_count;
    int total_duration;
} Melody;

typedef struct {
    MessageType type;
    char payload[BUFFER_SIZE];
    int timestamp;
} SerialMessage;

typedef struct {
    int song_id;
    int option1;
    int option2;
    int correct_answer;
} RoundData;

// =========================================================================
// GLOBAL VARIABLES
// =========================================================================

int serial_port = -1;
struct termios original_termios;
unsigned long message_counter = 0;

// =========================================================================
// FUNCTION PROTOTYPES
// =========================================================================

// Port Management
int open_serial_port(const char *port);
int close_serial_port(void);
int is_port_open(void);

// Message Sending
int send_message(SerialMessage *msg);
int send_start_signal(void);
int send_song_options(int option1, int option2);
int send_reset_signal(void);
int send_test_signal(void);

// Message Receiving
SerialMessage* receive_message(void);
int receive_with_timeout(char *buffer, int timeout);
MessageType parse_message_type(const char *data);

// Data Parsing
int parse_player_guess(const char *data, int *player_num, int *guess);
int parse_results(const char *data, int *p1, int *p2, int *correct);
int parse_song_options(const char *data, int *opt1, int *opt2);

// Melody Encoding/Decoding
char* encode_melody_to_string(Melody *melody);
Melody* decode_melody_from_string(const char *data);

// Protocol Utilities
char* get_message_type_string(MessageType type);
int validate_message_format(const char *data);
void print_protocol_status(void);

// =========================================================================
// PORT MANAGEMENT FUNCTIONS
// =========================================================================

int open_serial_port(const char *port) {
    printf("[SERIAL] Opening port: %s at 9600 baud\n", port);
    
    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        printf("[ERROR] Could not open port %s\n", port);
        return -1;
    }
    
    // Get current settings
    struct termios new_termios;
    tcgetattr(fd, &original_termios);
    new_termios = original_termios;
    
    // Set baud rate
    cfsetispeed(&new_termios, BAUD_RATE);
    cfsetospeed(&new_termios, BAUD_RATE);
    
    // 8 bit, 1 stop bit, no parity
    new_termios.c_cflag &= ~CSIZE;
    new_termios.c_cflag |= CS8;
    new_termios.c_cflag &= ~CSTOPB;
    new_termios.c_cflag &= ~PARENB;
    
    // No flow control
    new_termios.c_cflag |= CLOCAL | CREAD;
    new_termios.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    // Raw input
    new_termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_termios.c_oflag &= ~OPOST;
    
    // Non-blocking read with timeout
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 10;  // 1 second timeout
    
    tcsetattr(fd, TCSANOW, &new_termios);
    tcflush(fd, TCIOFLUSH);
    
    serial_port = fd;
    printf("[✓] Serial port opened successfully\n");
    return fd;
}

int close_serial_port(void) {
    if (serial_port >= 0) {
        tcsetattr(serial_port, TCSANOW, &original_termios);
        close(serial_port);
        printf("[✓] Serial port closed\n");
        serial_port = -1;
        return 1;
    }
    return 0;
}

int is_port_open(void) {
    return (serial_port >= 0) ? 1 : 0;
}

// =========================================================================
// MESSAGE SENDING FUNCTIONS
// =========================================================================

int send_message(SerialMessage *msg) {
    if (!is_port_open()) {
        printf("[ERROR] Serial port is not open\n");
        return -1;
    }
    
    char buffer[BUFFER_SIZE * 2];
    int bytes_written;
    
    // Format message: MSG_TYPE|PAYLOAD\n
    snprintf(buffer, sizeof(buffer) - 1, "%s|%s\n", 
        get_message_type_string(msg->type), 
        msg->payload);
    
    bytes_written = write(serial_port, buffer, strlen(buffer));
    
    if (bytes_written < 0) {
        printf("[ERROR] Failed to write to serial port\n");
        return -1;
    }
    
    printf("[SENT] (%d bytes) %s", bytes_written, buffer);
    message_counter++;
    return bytes_written;
}

int send_start_signal(void) {
    SerialMessage msg;
    msg.type = MSG_START;
    strcpy(msg.payload, "");
    return send_message(&msg);
}

int send_song_options(int option1, int option2) {
    SerialMessage msg;
    msg.type = MSG_OPTIONS;
    snprintf(msg.payload, BUFFER_SIZE, "%d,%d", option1, option2);
    return send_message(&msg);
}

int send_reset_signal(void) {
    SerialMessage msg;
    msg.type = MSG_RESET;
    strcpy(msg.payload, "");
    return send_message(&msg);
}

int send_test_signal(void) {
    SerialMessage msg;
    msg.type = MSG_TEST;
    strcpy(msg.payload, "PROTOCOL_TEST");
    return send_message(&msg);
}

// =========================================================================
// MESSAGE RECEIVING FUNCTIONS
// =========================================================================

SerialMessage* receive_message(void) {
    static SerialMessage msg;
    static char buffer[BUFFER_SIZE];
    
    if (!is_port_open()) {
        printf("[ERROR] Serial port is not open\n");
        return NULL;
    }
    
    int bytes = read(serial_port, buffer, BUFFER_SIZE - 1);
    
    if (bytes <= 0) {
        return NULL;
    }
    
    buffer[bytes] = '\0';
    printf("[RECEIVED] (%d bytes) %s\n", bytes, buffer);
    
    // Parse message
    msg.type = parse_message_type(buffer);
    
    // Extract payload (after first |)
    char *payload_start = strchr(buffer, '|');
    if (payload_start) {
        payload_start++;
        strncpy(msg.payload, payload_start, BUFFER_SIZE - 1);
        // Remove newline
        msg.payload[strcspn(msg.payload, "\n")] = 0;
    } else {
        strcpy(msg.payload, buffer);
    }
    
    msg.timestamp = (int)time(NULL);
    return &msg;
}

int receive_with_timeout(char *buffer, int timeout) {
    unsigned long start_time = time(NULL);
    
    while (time(NULL) - start_time < timeout) {
        int bytes = read(serial_port, buffer, BUFFER_SIZE - 1);
        
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("[RECEIVED] %s\n", buffer);
            return bytes;
        }
        
        usleep(50000);  // 50ms delay
    }
    
    printf("[TIMEOUT] No data received within %d seconds\n", timeout);
    return -1;
}

// =========================================================================
// MESSAGE PARSING FUNCTIONS
// =========================================================================

MessageType parse_message_type(const char *data) {
    if (strstr(data, "START")) return MSG_START;
    if (strstr(data, "#OPT:")) return MSG_OPTIONS;
    if (strstr(data, "P1_GUESS:")) return MSG_P1_GUESS;
    if (strstr(data, "P2_GUESS:")) return MSG_P2_GUESS;
    if (strstr(data, "P1:")) return MSG_RESULTS;
    if (strstr(data, "RESET")) return MSG_RESET;
    if (strstr(data, "TEST")) return MSG_TEST;
    if (strstr(data, "ACK")) return MSG_ACK;
    
    return MSG_ERROR;
}

int parse_player_guess(const char *data, int *player_num, int *guess) {
    if (strstr(data, "P1_GUESS:")) {
        *player_num = 1;
        return sscanf(data, "P1_GUESS:%d", guess);
    } else if (strstr(data, "P2_GUESS:")) {
        *player_num = 2;
        return sscanf(data, "P2_GUESS:%d", guess);
    }
    return 0;
}

int parse_results(const char *data, int *p1, int *p2, int *correct) {
    return sscanf(data, "P1:%d,P2:%d,CORRECT:%d", p1, p2, correct);
}

int parse_song_options(const char *data, int *opt1, int *opt2) {
    return sscanf(data, "#OPT:%d,%d", opt1, opt2);
}

// =========================================================================
// MELODY FUNCTIONS
// =========================================================================

char* encode_melody_to_string(Melody *melody) {
    static char encoded[BUFFER_SIZE];
    int pos = 0;
    
    pos += snprintf(encoded, BUFFER_SIZE, "MELODY:%d|", melody->melody_id);
    
    for (int i = 0; i < melody->note_count; i++) {
        pos += snprintf(encoded + pos, BUFFER_SIZE - pos, "%d,%d;",
            melody->notes[i].frequency,
            melody->notes[i].duration);
    }
    
    return encoded;
}

Melody* decode_melody_from_string(const char *data) {
    static Melody melody;
    memset(&melody, 0, sizeof(Melody));
    
    sscanf(data, "MELODY:%d|", &melody.melody_id);
    
    const char *notes_start = strchr(data, '|') + 1;
    int freq, dur;
    int note_count = 0;
    
    char temp[BUFFER_SIZE];
    strcpy(temp, notes_start);
    
    char *token = strtok(temp, ";");
    while (token && note_count < MAX_MELODY_NOTES) {
        if (sscanf(token, "%d,%d", &freq, &dur) == 2) {
            melody.notes[note_count].frequency = freq;
            melody.notes[note_count].duration = dur;
            melody.notes[note_count].id = note_count;
            note_count++;
        }
        token = strtok(NULL, ";");
    }
    
    melody.note_count = note_count;
    return &melody;
}

// =========================================================================
// PROTOCOL UTILITIES
// =========================================================================

char* get_message_type_string(MessageType type) {
    switch (type) {
        case MSG_START: return "START";
        case MSG_OPTIONS: return "#OPT";
        case MSG_P1_GUESS: return "P1_GUESS";
        case MSG_P2_GUESS: return "P2_GUESS";
        case MSG_RESULTS: return "RESULTS";
        case MSG_RESET: return "RESET";
        case MSG_TEST: return "TEST";
        case MSG_ACK: return "ACK";
        case MSG_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

int validate_message_format(const char *data) {
    if (!data || strlen(data) == 0) {
        printf("[WARN] Empty message\n");
        return 0;
    }
    
    // Basic validation - check if message has valid structure
    if (strchr(data, '|') == NULL && strchr(data, ':') == NULL) {
        printf("[WARN] Invalid message format: %s\n", data);
        return 0;
    }
    
    return 1;
}

void print_protocol_status(void) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                   SERIAL PROTOCOL STATUS                       ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║ Port Status: %s\n", is_port_open() ? "OPEN" : "CLOSED");
    printf("║ Messages Sent: %lu\n", message_counter);
    printf("║ Protocol Version: 1.0\n");
    printf("║ Baud Rate: 9600\n");
    printf("║ Data Bits: 8\n");
    printf("║ Stop Bits: 1\n");
    printf("║ Parity: None\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
}

// =========================================================================
// COMMUNICATION EXAMPLES
// =========================================================================

/*
// Example: Full round communication cycle

void example_round_communication() {
    // 1. Open port
    open_serial_port("/dev/ttyUSB0");
    
    // 2. Send START signal
    send_start_signal();
    sleep(1);
    
    // 3. Send song options
    send_song_options(1, 3);
    sleep(1);
    
    // 4. Wait for player guesses
    unsigned long start = time(NULL);
    int p1_received = 0, p2_received = 0;
    
    while (time(NULL) - start < 30) {
        SerialMessage *msg = receive_message();
        
        if (msg) {
            if (msg->type == MSG_P1_GUESS) {
                int player, guess;
                parse_player_guess(msg->payload, &player, &guess);
                printf("Player %d guessed: %d\n", player, guess);
                p1_received = 1;
            } else if (msg->type == MSG_P2_GUESS) {
                int player, guess;
                parse_player_guess(msg->payload, &player, &guess);
                printf("Player %d guessed: %d\n", player, guess);
                p2_received = 1;
            } else if (msg->type == MSG_RESULTS) {
                int p1, p2, correct;
                parse_results(msg->payload, &p1, &p2, &correct);
                printf("Results: P1=%d, P2=%d, Correct=%d\n", p1, p2, correct);
                break;
            }
        }
        
        usleep(50000);
    }
    
    // 5. Close port
    close_serial_port();
    print_protocol_status();
}
*/
