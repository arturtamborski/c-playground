//╔══════════════════════════════════════════════════════════════════════════════════════════╗
//║ main.h                                                                                   ║
//║ 16 cze 2014                                                                              ║
//║ by MrWeb123                                                                              ║
//╚══════════════════════════════════════════════════════════════════════════════════════════╝
#ifndef MAIN_H_
#define MAIN_H_

//w razie problemów z odczytem, zmień ścieżkę dostępu do plików
#define DIRECTORY_XO "./tabxo.txt"
#define DIRECTORY_LA "./tablarge.txt"

#define C2I(c) (c - '0') //zamienia cyfrę char na cyfrę int (patrz tablica ASCII)

#define A_DEFAULT 0
#define A_BRIGHT 1
#define A_UNDERLINE_ON 4
#define A_BLINK_ON 5
#define A_NEGATIVE_ON 7
#define A_INVISIBLE 8
#define A_VISIBLE 22
#define A_UNDERLINE_OFF 24
#define A_BLINK_OFF 25
#define A_NEGATIVE_OFF 27

#define C_BLACK 30
#define C_RED 31
#define C_GREEN 32
#define C_YELLOW 33
#define C_BLUE 34
#define C_MAGNETA 35
#define C_CYAN 36
#define C_WHITE 37
#define C_DEFAULT 39

#define C_RESET cout << "%c[dm"
#define C_BELL cout << "\a"


#endif /* MAIN_H_ */
