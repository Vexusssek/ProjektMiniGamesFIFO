
# Projekt MiniGamesFIFO

Projekt ten został stworzony w ramach przedmiotu 'Systemy Operacyjne' i zawiera trzy proste, tekstowe mini-gry, które można uruchomić na dwóch terminalach jednocześnie. Każda z gier została zaimplementowana w osobnym pliku: rockpaperscissors.c, tictactoe.c oraz guessthenumber.c.
Pisząc ten program chciałem użyć elementów, które pojawiły się na zajęciach - stąd FIFO.

*Przykładowa kompilacja i uruchomienie*

```
gcc -o game main.c rockpaperscissors.c tictactoe.c guessthenumber.c
```

**Terminal 1**
```
./game 1
```
**Terminal 2**
```
./game 2
```
