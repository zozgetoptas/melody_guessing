#include <stdio.h>

#define LILA      "\033[38;5;141m"
#define PINK      "\033[38;5;206m"
#define RESET     "\033[0m"
#define BOLD      "\033[1m"

int main() {
    printf("\e[1;1H\e[2J\n\n");

    // --- KUSURSUZ BOOMBOX ---
    printf(LILA "                     ._________________________________________. \n");
    printf(LILA "                     | " PINK ":::       MELODY BOOMBOX       :::" LILA "     | \n");
    printf(LILA "                     |_________________________________________| \n");
    printf(LILA "                     |   " PINK "___" LILA "   |                           |   " PINK "___" LILA "   | \n");
    printf(LILA "                     |  " PINK "/###\\" LILA "  |  " PINK "_________________________" LILA "  |  " PINK "/###\\" LILA "  | \n");
    printf(LILA "                     | " PINK "|#####|" LILA " | | " PINK " _____________________ " LILA " | | " PINK "|#####|" LILA " | \n");
    printf(LILA "                     | " PINK "|#####|" LILA " | | " PINK "|_____________________|" LILA " | | " PINK "|#####|" LILA " | \n");
    printf(LILA "                     |  " PINK "\\###/" LILA "  | | " PINK "|_____________________|" LILA " | |  " PINK "\\###/" LILA "  | \n");
    printf(LILA "                     |   " PINK "---" LILA "   |  " PINK "-------------------------" LILA "  |   " PINK "---" LILA "   | \n");
    printf(LILA "                     |_________|___________________________|_________| \n");
    printf(LILA "                     | " PINK "[=] [=]" LILA "  |   " PINK "o o o o o o o o o" LILA "   |  " PINK "[=] [=]" LILA " | \n");
    printf(LILA "                     |___________" PINK "  B A T T L E  M O D E  " LILA "___________| \n" RESET);

    // --- BAŞLIK ---
    printf("\n                                  " PINK BOLD "« ADMIN CONTROL PANEL »" RESET "\n\n");

    // --- MENÜ KUTUSU ---
    printf(LILA "                        ┌──────────────────────────────────────────┐\n");
    printf(LILA "                        │ " PINK BOLD "        - CONTROL INTERFACE -       " LILA "   │\n");
    printf(LILA "                        ├──────────────────────────────────────────┤\n");
    printf(LILA "                        │                                          │\n");
    printf(LILA "                        │  " PINK "◈" LILA " [1] " RESET "INITIATE NEW SESSION      " LILA "   │\n");
    printf(LILA "                        │  " PINK "◈" LILA " [2] " RESET "DIFFICULTY CONFIGURATION  " LILA "   │\n");
    printf(LILA "                        │  " PINK "◈" LILA " [3] " RESET "SYSTEM SETTINGS           " LILA "   │\n");
    printf(LILA "                        │  " PINK "◈" LILA " [4] " RESET "GLOBAL RANKINGS           " LILA "   │\n");
    printf(LILA "                        │  " PINK "◈" LILA " [5] " RESET "FACTORY RESET             " LILA "   │\n");
    printf(LILA "                        │  " PINK "◈" PINK " [6] TERMINATE                 " LILA "   │\n");
    printf(LILA "                        │                                          │\n");
    printf(LILA "                        └──────────────────────────────────────────┘\n" RESET);

    printf("\n\n                        " PINK "» " LILA "ACCESS CODE: " RESET);

    return 0;
}