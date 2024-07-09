#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

extern void write_log(const char *message);

// Makra kolorów, pliki FIFO, "czyszczenie" ekranu
#define FIFO1 "tictactoe_fifo1"
#define FIFO2 "tictactoe_fifo2"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define CLEAR_SCREEN "\033[2J\033[H"

// Wyświetlanie stanu planszy na ekranie
void print_board(char board[3][3])
{
    printf("\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Sprawdzenie wygranego gracza
int find_winner(char board[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        // Wiersze i kolumny
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '.') ||
            (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '.'))
        {
            return (board[i][0] == 'X' || board[0][i] == 'X') ? 1 : 2;
        }
    }
    // Przekątne
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '.') ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '.'))
    {
        return (board[1][1] == 'X') ? 1 : 2;
    }

    // Czy gra nadal trwa?
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == '.')
            {
                return 0;
            }
        }
    }
    return 3; // REMIS
}

// Wysyłanie ruchu do przeciwnika
void send_move(int f, int x, int y)
{
    int message[2] = {x, y};
    write(f, message, sizeof(message));
}

// Odbieranie ruchu od przeciwnika
void receive_move(int f, int *x, int *y)
{
    int message[2];
    read(f, message, sizeof(message));
    *x = message[0];
    *y = message[1];
}

void start_tictactoe(int player)
{
    // Zmienne, tablica, status ruchu, "czyszczenie" ekranu, nagłówek gry
    int f1, f2;
    char board[3][3];
    int x, y, winner = 0;

    printf(CLEAR_SCREEN);
    printf(RED"=======================================\n"RESET);
    printf(CYAN"              KÓŁKO I KRZYŻYK\n");
    printf(RED"=======================================\n"RESET);
    printf("\n");

    //Inicjalizacja planszy (pustej) i utworzenie plików FIFO
    memset(board, '.', sizeof(board));
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    /*Pętla do momentu znalezienia zwycięzcy, kod dla gracza 1 i 2 (wyświetlanie planszy, wolne pola, ruch, zwycięzca, plansza po ruchu, wysyłanie ruchu,
     oczekiwanie na ruch przeciwnika, odczytanie ruchu, aktualizacja zwycięzcy, wyświetlenie planszy po zaktalizowaniu )*/
    while (winner == 0)
    {
        if (player == 1)
        {
            print_board(board);
            printf("\n");
            printf("Gracz 1 (X), podaj współrzędne (x y): \n");
            scanf("%d %d", &x, &y);

            while (board[x][y] != '.')
            {
                printf("To pole jest już zajęte. Podaj inne współrzędne (x y): \n");
                scanf("%d %d", &x, &y);
            }

            board[x][y] = 'X';
            winner = find_winner(board);
            print_board(board);

            f1 = open(FIFO1, O_WRONLY);
            send_move(f1, x, y);
            close(f1);

            if (winner != 0) break;

            printf("Oczekiwanie na ruch gracza 2...\n");
            f2 = open(FIFO2, O_RDONLY);
            receive_move(f2, &x, &y);
            close(f2);

            board[x][y] = 'O';
            winner = find_winner(board);
        }
        else
        {
            printf("Oczekiwanie na ruch gracza 1...\n");
            f1 = open(FIFO1, O_RDONLY);
            receive_move(f1, &x, &y);
            close(f1);

            board[x][y] = 'X';
            winner = find_winner(board);
            print_board(board);

            if (winner != 0) break;

            printf("Gracz 2 (O), podaj współrzędne (x y): ");
            scanf("%d %d", &x, &y);

            while (board[x][y] != '.')
            {
                printf("To pole jest już zajęte. Podaj inne współrzędne (x y): ");
                scanf("%d %d", &x, &y);
            }

            board[x][y] = 'O';
            f2 = open(FIFO2, O_WRONLY);
            send_move(f2, x, y);
            close(f2);
            print_board(board);
            winner = find_winner(board);
        }
    }

    //Komunikaty o wygranej, zapisy do logów
    const char *result_message = (winner == 1) ? "Gracz X wygrywa!" : (winner == 2) ? "Gracz O wygrywa!" : "Gra w papier-kamień-nożyce zakończyła się remisem!";

    printf(RED"\n%s\n"RESET, result_message);
    write_log(result_message);
    printf("\nNaciśnij Enter, aby kontynuować...");
    while (getchar() != '\n');
    getchar();
    printf(CLEAR_SCREEN);
}
