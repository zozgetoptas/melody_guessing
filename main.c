#include "melody_guessing.h"

int main()
{
    int choice;
    
    srand(time(NULL));
    
    printf("\n");

    load_song_database();
    
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
                change_difficulty();        
                break;
            case 3: 
                view_settings();            
                break;
            case 4: 
                display_scoreboard();       
                break;
            case 5:
                reset_game();               
                break;
            case 6:
                printf("\n[*] Exiting Admin Console. Goodbye!\n\n");
                return 0;
            default:
                printf("[!] Invalid choice (1-6).\n");
        }
    }
    return 0;
}