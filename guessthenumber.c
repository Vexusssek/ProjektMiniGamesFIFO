#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

extern void write_log(const char *message);

//makra kolorów, nazwy FIFO, "czyszczenie" ekranu
#define FIFO_FILE "guess_fifo"
#define FIFO_RESULT "guess_fifo_result"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define PURPLE  "\033[1;35m"
#define CLEAR_SCREEN "\033[2J\033[H"

void start_guess_the_number(int player)
{
    //Czyszczenie ekranu, wyświetlanie nagłówka gry
    printf(CLEAR_SCREEN);
    fflush(stdout);
    printf(RED"=======================================\n");
    printf(YELLOW"           ZGADNIJ LICZBĘ\n");
    printf(RED"=======================================\n"RESET);

    //Zmienne do obsługi FIFO, wybranych liczb do odgadnięcia, zgadywanych liczb, liczby prób, wyniku do logów
    int f, f_result;
    int number, guess = 0, attempts;
    char log_message[100];

    //Tworzenie plików FIFO
    mkfifo(FIFO_FILE, 0666);
    mkfifo(FIFO_RESULT, 0666);

    /*Pierwszy gracz oczekuje na gracza numer dwa, po czym otwiera FIFO do zapisu, pobiera liczbę do zgadnięcia oraz lizbę prób. Wartości są
    zapisywane. Gracz 1 oczekuje na wyniki gry Gracza 2*/
    if (player == 1)
    {
        printf(PURPLE"Oczekiwanie na Gracza 2...\n"RESET);
        f = open(FIFO_FILE, O_WRONLY);
        printf("Graczu numer 1, podaj liczbę do zgadnięcia: ");
        scanf("%d", &number);
        printf("Graczu numer 1, podaj liczbę prób: ");
        scanf("%d", &attempts);
        write(f, &number, sizeof(int));
        write(f, &attempts, sizeof(int));
        close(f);

        f_result = open(FIFO_RESULT, O_RDONLY);
        read(f_result, &guess, sizeof(int));
        close(f_result);
    }
    /*Oczekiwanie na gracza pierwszego, otwarcie FIFO, odczytanie ilości prób, liczby do zgadnięcia. Odgadywanie liczby, oznaczenie flagi, zapis do logów,
    zapis do FIFO*/
    else
    {
        printf(PURPLE"Oczekiwanie na Gracza 1...\n"RESET);
        f = open(FIFO_FILE, O_RDONLY);
        read(f, &number, sizeof(int));
        read(f, &attempts, sizeof(int));
        close(f);

        int start_attempts = attempts;
        while (attempts-- > 0)
        {
            printf("Graczu numer 2, zgadnij liczbę: ");
            scanf("%d", &guess);
            if (guess == number)
            {
                snprintf(log_message, sizeof(log_message), "Gracz 2 odgadł liczbę po liczbie prób: %d", start_attempts - attempts);
                write_log(log_message);
                guess = 1;
                break;
            }
            else if (guess < number)
            {
                printf("Liczba jest większa.\n");
            } else
            {
                printf("Liczba jest mniejsza.\n");
            }
        }

        f_result = open(FIFO_RESULT, O_WRONLY);
        write(f_result, &guess, sizeof(int));
        close(f_result);
    }

    // Wyświetlanie wyniku
    if (guess != 1)
    {
        if (player == 2)
        {
            printf(RED"\nPrzegrałeś! Naciśnij Enter, aby kontynuować...\n"RESET);
        }
        else
        {
            printf(RED"\nGracz 2 przegrał! Naciśnij Enter, aby kontynuować...\n"RESET);
        }
        while (getchar() != '\n');
        getchar();
        printf(CLEAR_SCREEN);
        fflush(stdout);
    }
    if (guess == 1)
    {
        if(player == 1)
        {
            printf(RED"\nGracz 2 odgadł liczbę! Naciśnij Enter, aby kontynuować...\n"RESET);
        }
        else
        {
            printf(RED"\nBrawo, odgadłeś liczbę! Naciśnij Enter, aby kontynuować...\n"RESET);
        }
        while (getchar() != '\n');
        getchar();
        printf(CLEAR_SCREEN);
        fflush(stdout);
    }

    // Usunięcie plików
    unlink(FIFO_FILE);
    unlink(FIFO_RESULT);
}
