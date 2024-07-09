#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//makra kolorów, "czyszczenie" ekranu
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define PURPLE  "\033[1;35m"
#define CLEAR_SCREEN "\033[2J\033[H"

//funkcje poszczególnych mini-gier
void start_rock_paper_scissors(int player);
void start_guess_the_number(int player);
void start_tictactoe(int player);

//funkcja obsługująca logi (zapisuje do pliku "game_results.txt")
void write_log(const char *message)
{
    FILE *file = fopen("game_results.txt", "a");
    if (file == NULL)
    {
        perror("Błąd!");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now); //konwertuje aktualny czas na strukture tm (rok,miesiąc,dzień,godzina...)

    fprintf(file, "%02d-%02d-%02d %02d:%02d - %s\n", t->tm_year+1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, message);
    fclose(file);
}

int main(int argc, char *argv[])
{

    int player = atoi(argv[1]);
    int choice;

    do
    {
        printf(CLEAR_SCREEN);
        printf("\n");
        printf(RED"=======================================\n");
        printf(YELLOW"           MENU MINI-GIER\n");
        printf(RED"=======================================\n");
        printf(CYAN"1. Papier-kamień-nożyce\n");
        printf(CYAN"2. Zgadnij liczbę\n");
        printf(CYAN"3. Kółko i krzyżyk\n");
        printf(CYAN"4. Wyjście\n");
        printf(RED"=======================================\n");
        printf(PURPLE"WYBÓR> "RESET);
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                start_rock_paper_scissors(player);
                break;
            case 2:
                start_guess_the_number(player);
                break;
            case 3:
                start_tictactoe(player);
                break;
            case 4:
                printf("Wyjście z gry.\n");
                break;
            default:
                printf("Nieprawidłowy wybór.\n");
        }


    }
    while (choice != 4);

    return 0;
}
