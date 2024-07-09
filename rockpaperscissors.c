#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

extern void write_log(const char *message);

//makra kolorów, "czyszczenie" ekranu, nazwy FIFO
#define FIFO1 "rps_fifo1"
#define FIFO2 "rps_fifo2"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CLEAR_SCREEN "\033[2J\033[H"

void start_rock_paper_scissors(int player)
{
    //czyszczenie ekranu i wyświetlanie nagłówka gry
    printf(CLEAR_SCREEN);
    printf(RED"=======================================\n");
    printf(CYAN"           PAPIER-KAMIEŃ-NOŻYCE\n");
    printf(RED"=======================================\n"RESET);
    printf("\n");

    // zmienne do obsługi FIFO, opcji wybieranych przez graczy, komunikatu do logów
    int f1, f2;
    char player_choice, opponent_choice;
    char fifo_write[10], fifo_read[10];
    char log_message[100];

    //Ustawianie nazw plików FIFO dla graczy (odczytywanie, zapisywanie, komunikacja)
    if (player == 1)
    {
        printf("Oczekiwanie na gracza 2...\n");
        printf("\n");
        strcpy(fifo_write, FIFO1);
        strcpy(fifo_read, FIFO2);
    }
    else
    {
        printf("Oczekiwanie na gracza 1...\n");
        printf("\n");
        strcpy(fifo_write, FIFO2);
        strcpy(fifo_read, FIFO1);
    }

    // Tworzenie plików FIFO
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    //Gracz 1 wybiera swoją opcję, która zapisuje się do FIFO1, odczytuje wybraną opcję przeciwnika (FIFO2). Gracz 2 odczytuje opcję przeciwnika, wybiera swoją opcję
    //i zapisuje do FIFO2
    if (player == 1)
    {
        f1 = open(fifo_write, O_WRONLY);
        f2 = open(fifo_read, O_RDONLY);
        do
        {
            printf("Graczu numer 1, wybierz" CYAN" p - papier, " GREEN"k - kamień, " RED"n - nożyce: "RESET);
            scanf(" %c", &player_choice);
            if (player_choice != 'p' && player_choice != 'k' && player_choice != 'n')
            {
                printf(RED"Niepoprawny wybór!\n"RESET);
            }
        }
        while (player_choice != 'p' && player_choice != 'k' && player_choice != 'n');
        write(f1, &player_choice, sizeof(char));
        read(f2, &opponent_choice, sizeof(char));
    }
    else
    {
        f1 = open(fifo_read, O_RDONLY);
        f2 = open(fifo_write, O_WRONLY);
        read(f1, &player_choice, sizeof(char));
        do
        {
            printf("Graczu numer 2, wybierz" CYAN" p - papier, " GREEN"k - kamień, " RED"n - nożyce: "RESET);
            scanf(" %c", &opponent_choice);
            if (opponent_choice != 'p' && opponent_choice != 'k' && opponent_choice != 'n')
            {
                printf(RED"Niepoprawny wybór!\n"RESET);
            }
        }
        while (opponent_choice != 'p' && opponent_choice != 'k' && opponent_choice != 'n');
        write(f2, &opponent_choice, sizeof(char));
    }

    //Wynik gry, zapisanie do logów
    char result[50];
    if (player_choice == opponent_choice)
    {
        snprintf(result, sizeof(result),CYAN"\nREMIS!"RESET);
        if (player == 1)
        {
            write_log("Gra w kamień-papier-nożyce zakończyła się remisem.");
        }
    }
    else if ((player_choice == 'p' && opponent_choice == 'k') ||
        (player_choice == 'k' && opponent_choice == 'n') ||
        (player_choice == 'n' && opponent_choice == 'p'))
    {
        snprintf(result, sizeof(result), CYAN "\n WYGRYWA GRACZ NUMER 1!" RESET);
    if (player == 1)
    {
        write_log("Gracz 1 wygrał w kamień-papier-nożyce.");
    }
    }
        else
        {
            snprintf(result, sizeof(result), CYAN "\n WYGRYWA GRACZ NUMER 2!" RESET);
            if (player == 2)
            {
                write_log("Gracz 2 wygrał w kamień-papier-nożyce.");
            }
        }

        //Wyświetlanie wyniku
        if (player == 1)
        {
            printf("%s\n", result);
            write(f1, result, sizeof(result));
            printf("\nNaciśnij Enter, aby kontynuować...");
            while (getchar() != '\n');
            getchar();
            printf(CLEAR_SCREEN);
        }
        else
        {
            read(f1, result, sizeof(result));
            printf("%s\n", result);
            printf("\nNaciśnij Enter, aby kontynuować...");
            while (getchar() != '\n');
            getchar();
            printf(CLEAR_SCREEN);;
        }


        //Zamknięcie, usunięcie FIFO
        close(f1);
        close(f2);
        unlink(FIFO1);
        unlink(FIFO2);
}
