//╔══════════════════════════════════════════════════════════════════════════════════════════╗
//║ main.cpp                                                                                 ║
//║ 16 cze 2014                                                                              ║
//║ by MrWeb123                                                                              ║
//╚══════════════════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <stdio.h>

#include <termios.h>
#include <unistd.h>

#include "main.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////
int board[10]; //tablica zawiera układ pól (1-9) i stan każdego pola (0-nic, 1-plr, 2-cpu)

int wylosowane[10]; //zawiera wylosowane liczby przez CPU
int ridx = 1;      //zawiera indeks do którego ostatnio zapisano wylosowaną liczbę

string tabxo[6];      //tablica zawiera odczytany symbol z pliku tabxo.txt
string tablarge[22]; //tablica zawiera odczytaną ramkę z pliku tablarge.txt

string a;          //śmieciowa zmienna używana tylko do wstrzymania programu albo debugowania
int line = 1;     //zmiennna pomocnicza, wykorzystana przy odczycie linii z pliku tekstowego
bool file_error; // 0 - brak pliku/ów , 1 - wszystko jest ok

int ticks;    //takty gry (zmienna potrzebna dla rules, gdy nikt nie zdobył pkt )
int lvl = 1; //poziom trudności komputera i wyznacznik aktualnego poziomu gry
char yesno; //decyzja wybrana przez gracza przy pytaniach o np. kontynuację gry

int pkt_plr;   //punkty gracza
int pkt_cpu;  //punkty komputera
int pkt_get; //ostatnio zdobyty punkt (0 - nikt, 1 - plr, 2 - cpu)

bool again;   //1 - włącz grę jeszcze raz, 0 - wyłącz program
bool player; //0 - plr, 1 - cpu

//////////////////////////////////////////////////////////////////////////////////////////////
char getch( bool _show ); //odpowiednik windowsowego getch z biblioteki conio.h
void c_clr( int _line ); //czyści podaną linię z konsoli (po wpisaniu 0 wyczyści całe okno)
void c_col(int _attr, int _fcolor, int _bcolor ); //ustawia kolor czcionki i tła w konsoli
void c_pos( int _x, int _y );                     //ustawia pozycję kursora w konsoli

void check_files( void ); //sprawdza czy pliki są dołączone do projektu

void read_tabxo( char _xo );    //odczytuje wybrany znak z pliku i wpisuje go do tablicy tabxo
void read_table( int _table ); //odczytuje tablicę do wyświetlenia

void draw_tabxo( char _xo, int _yesno ); //rysuje x albo o na podanej pozycji
void draw_table( int _table );          //rysuje tabelę

int  ai_rnd( void );      //losowanie liczb (1-9) dla yesnotawienia X:O przez CPU
void ai_cpu( int _lvl ); //"inteligencja" cpu w grze
void ai_plr( void );    //obsługa gracza

void rules( void );  //sprawdza czy gra prowadzona jest wg zasad
void play(  void ); //rozpoczyna grę

//////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
  play();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
char getch( bool _show )
{
      ////////////////////////////////////////////////////////////////
     /// PAMIĘTAJ O DODANIU TYCH BIBLIOTEK DLA DZIAŁANIA FUNKCJI! ///
    ///  #include <termios.h>                                    ///
   ///   #include <unistd.h>                                    ///
  ////////////////////////////////////////////////////////////////
  char buf = 0;

  struct termios old={0};

  fflush(stdout);

  if(tcgetattr(0, &old) < 0)
    perror("tcsetattr()");

  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  if(tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");

  if(read(0, &buf, 1) < 0)
    perror("read()");

  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;

  if(tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");

  if(_show == true) cout << buf;
  return buf;
}


void c_clr( int _line )
{
  if(_line == 0) //podaj 0 jako parametr dla wyczyszczenia całego ekranu
  {
    for(int i = 0; i <= 24; i++)
    {
      c_pos(i, 0); //ustaw kursor na wybranej linii
      cout << "\e[K"; //usuń linię
    }
  }

  else //jakakowiek inna cyfra większa od 0 będzie oznaczała wymazanie danej linii

  c_pos(_line, 0); //ustaw kursor na wybranej linii
  cout << "\e[K"; //usuń linię
}


void c_col(int _attr, int _fcolor, int _bcolor )
{
  //ESCAPE SEQUENCE ESC[1;32m = jasny zielony, ESC[0;32m zwykły zielony
  cout << "\e[" << _attr << ';' <<_fcolor << 'm';
  cout << "\e[" << _bcolor + 10 << 'm'; //+ 10 ponieważ wykorzystuje te same deklaracje
  //dla foreground i background
}


void c_pos( int _x, int _y )
{
  //ESCAPE SEQUENCE ESC[1;1f ustawi kursor w lewym górnym rogu
  cout << "\e[" << _x << ';' << _y << 'f';
}

//////////////////////////////////////////////////////////////////////////////////////////////
void check_files( void )
{
  fstream plik; //zmienna pomocnicza do otwierania plików

  plik.open(DIRECTORY_XO, ios::in);
  if(plik.good() == false) //jeśli nie udało się otworzyć pliku
    {
      file_error = true;
    }
  plik.close(); //zamknij plik

  plik.open(DIRECTORY_LA, ios::in );
  if(plik.good() == false) //jeśli nie udało się otworzyć pliku
    {
      file_error = true;
    }
  plik.close(); //zamknij plik

  if(file_error == true)
    {
      c_clr(0);
      c_pos(0, 1); c_col(A_BRIGHT, C_RED, C_DEFAULT); c_clr(1);
      cout << "error, missing file(s)";
      c_pos(0, 2); c_col(A_BRIGHT, C_RED, C_DEFAULT); c_clr(2);
      cout << "check it and try again!";
      cout << endl;
    }
}


void read_tabxo( char _xo )
{
  fstream plik;  //zmienna wykorzystywana do odczytu pliku
  string linia; //aktualnie odczytywana linia

  plik.open(DIRECTORY_XO, ios::in);
  plik.seekg(2, ios::beg);      //dwa znaki (LF CR) trzeba je pominąć
  if(_xo == 'x' || _xo == 'X') //jeśli chcesz odczytać X z pliku
    {
      for(int i = 0; i <= 5 && getline(plik, linia); i++) tabxo[i] = linia;
      plik.close();
    }

  else

  if(_xo == 'o' || _xo == 'O')     //jeśli chcesz odczytać O z pliku
    {
      plik.seekg(140, ios::beg); //tyle znaków trzeba pominąć dla odczytania znaku O
      for(int i = 0; i <= 5 && getline(plik, linia); i++) tabxo[i] = linia;
      plik.close();
    }

  else
    plik.close();
}


void read_table( int _table )
{
  fstream plik;
  string linia;

  //wybrane okno do odczytu i początek tego okna w pliku .txt
  if(_table == 1) line = 0;  else
  if(_table == 2) line = 1286; else
  if(_table > 2 || _table < 1) cout << "nie znaleziono okna";

  plik.open(DIRECTORY_LA, ios::in);
  plik.seekg(line, ios::beg); //przesunięcie kursora do odczytu
  for(int i = 0; i <= 22 && getline(plik, linia); i++) tablarge[i] = linia;
  plik.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////
void draw_tabxo( char _xo, int _poz )
{
  int x, y;

  if(_poz == 1) { x = 2;  y = 3;  } else
  if(_poz == 2) { x = 2;  y = 14; } else
  if(_poz == 3) { x = 2;  y = 25; } else
  if(_poz == 4) { x = 9;  y = 3;  } else
  if(_poz == 5) { x = 9;  y = 14; } else
  if(_poz == 6) { x = 9;  y = 25; } else
  if(_poz == 7) { x = 16; y = 3;  } else
  if(_poz == 8) { x = 16; y = 14; } else
  if(_poz == 9) { x = 16; y = 25; }

  read_tabxo( _xo ); //wczytaj X albo O do pamięci

  for(int i = 0; i <= 5; i++)
    {
      c_pos(x++, y); //wczytany znak zostanie wyświetlony od pozycji x, y
      cout << tabxo[i] << endl;
    }
}


void draw_table( int _table )
{
  read_table( _table ); //wczytaj podaną tablicę do pamięci

  c_clr(0);
  c_pos(0, 0);

  for(int i = 0; i <= 22; i++)
    {
      c_pos(i, 0);
      cout << tablarge[i] << endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
int ai_rnd( void )
{
  bool check = false; //0 - nie sprawdzono, 1 - sprawdzono
  int rnumber;       //przechowuje ostatnio wylosowaną cyfrę

  while(check == false)
    {
      rnumber = rand()%9 + 1; //jeśli nadal w funkcji, wylosuj cyfrę

    //dopuki i <= 9 albo rnumber jest różny od wylosowanych cyfr albo plansza jest wolna
      for(int i = 1; i <= 9 && rnumber != wylosowane[i] && board[rnumber] == 0 ; i++)
	{
	  if(i == 9) //jeśli już koniec pętli (koniec filtrów)
	    {
	      check = true;                  //wyjdź z losowania przerywając while(check == true)
	      wylosowane[ridx++] = rnumber; //zapisz do tablicy żeby nie powtórzyć
	      board[rnumber] = 2;          //zajmij pozycję na planszy
	    }
	}
    }
  return rnumber;
}


void ai_cpu( int _lvl )
{
  if(_lvl == 1)
    {
      int time = 150000; //szybkość animacji

      //Animacja "myślenia" CPU
      c_col(A_BRIGHT, C_YELLOW, C_BLUE);
      c_clr(24); c_pos(24, 0); cout << "|"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "/"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "\u2014" << flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "\\"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "|"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "/"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "\u2014" << flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "\\"	<< flush; usleep(time);
      c_clr(24); c_pos(24, 0); cout << "!"	<< flush; usleep(time);

      c_col(A_BRIGHT, C_RED, C_BLUE);
      draw_tabxo('o', ai_rnd());
    }

  else

  if(_lvl >= 2) { } // w wersji beta jest tylko jeden poziom trudności :(
  //nie mam pomysłu na więcej strategii... Sprawdzę w internecie i może kiedyś wrzucę
}


void ai_plr( void )
{
  bool good_pos = false;

  c_clr(23);
  c_pos(23, 0);
  c_col(A_BRIGHT, C_WHITE, C_BLUE);		cout << "Gdzie postawić znak ? ";
  c_clr(24);
  c_pos(24, 0);

  while(good_pos == false)
    {
      c_pos(23, 22);
      cout << "        ";
      c_pos(23,23);
      c_col(A_BRIGHT, C_YELLOW, C_BLUE);
      yesno = getch(1);

      if(yesno > '9' || yesno < '1')
        {
          c_clr(24);
          c_pos(24, 0);
          c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "Nieprawidłowy znak! ";
          C_BELL;
        }

      else

      if(board[C2I(yesno)] == 1 || board[C2I(yesno)] == 2)
        {
          c_clr(24);
          c_pos(24, 0);
          c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "To pole jest zajęte! ";
          C_BELL;
        }

      else

      if(board[C2I(yesno)] == 0)
	{
	  board[C2I(yesno)] = 1; //zapisz pozycję
	  c_col(A_BRIGHT, C_GREEN, C_BLUE);
	  draw_tabxo('x', C2I(yesno));
	  good_pos = true;
	}
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void rules( void )
{
  for(int i = 1; i < 3; i++)//skanuje wszystkie możliwości dla plr(1) i cpu(2)
    {
      //skan planszy
      if( (board[1] == i && board[2] == i && board[3] == i)
	||(board[4] == i && board[5] == i && board[6] == i)
	||(board[7] == i && board[8] == i && board[9] == i)

	||(board[1] == i && board[4] == i && board[7] == i)
	||(board[2] == i && board[5] == i && board[8] == i)
	||(board[3] == i && board[6] == i && board[9] == i)

	||(board[1] == i && board[5] == i && board[9] == i)
	||(board[3] == i && board[5] == i && board[7] == i) ) pkt_get = i;

      if(pkt_get == 1) { pkt_plr++; } else
      if(pkt_get == 2) { pkt_cpu++; }

      //jeśli ktoś zdobył pkt albo wszystkie pola są już zajęte
      if(pkt_get == 1 || pkt_get == 2 || ticks == 9)
	{
	  bool next = false;
	  pkt_get = 0;

	  c_clr(23);
	  c_pos(23, 0);
	  c_col(A_BRIGHT, C_WHITE, C_BLUE); 	cout << "Koniec gry! Grać dalej ? ";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "T";
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);	cout << "/";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "N";

	  c_clr(24);
	  c_pos(24, 0);
	  c_col(A_BRIGHT, C_WHITE, C_BLUE); 	cout << "Twoje punkty = ";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE); 	cout << pkt_plr;

	  c_pos(24, 20);
	  c_col(A_BRIGHT, C_WHITE, C_BLUE); 	cout << "Punkty CPU = ";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE); 	cout << pkt_cpu;

	  while(next == false)
	    {
	      c_pos(23, 29); cout << "      ";
	      c_col(A_BRIGHT, C_YELLOW, C_BLUE);
	      c_pos(23, 30);
	      yesno = getch(1);

	      if(yesno == 'T' || yesno == 't')
		{
		  next = true;
		  again = true;
		  player ^= 1; //zamiana kolejności w następnej grze, raz plr raz cpu
		  play();
		}

	      else

	      if(yesno == 'N' || yesno == 'n')
		{
		  next = true;
		  again = false;
		  c_col(A_DEFAULT, C_DEFAULT, C_DEFAULT); //kolor konsoli na default
		  c_clr(0);
		  c_pos(0, 0);
		  exit(1); //funkcja wyłączy program
		}

	      else C_BELL;
	    }
	}
    }
}


void play( void )
{
  bool next = false; //decyduje o wyjściu z pętli while

  check_files();   //sprawdź pliki

  if(file_error == false) //gra będzie kontynuowana jeśli nie wystąpiły błędy z plikami
    {
      //czyszczenie zmiennych. Wymagane ze względu na rekurencję funkcji play()
      for(int i = 0; i < 10; i++) { board[i] = 0; wylosowane[i] = 0; } ridx = 1; ticks = 0;

      if(again == false) //Splash screen wyświetla się tylko raz
	{
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);
	  draw_table(2); //narysuj ekran do dopasowania konsoli

	  c_pos(23, 0);
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);	cout << "Chcesz grać pierwszy? ";
	  c_pos(24,0);
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);	cout << "Wpisz ";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "T";
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);	cout << "ak lub ";
	  c_col(A_BRIGHT, C_YELLOW, C_BLUE);	cout << "N";
	  c_col(A_BRIGHT, C_WHITE, C_BLUE);	cout << "ie ";

	  while(next == false) //wszystkie odpowiedzi oprócz t/n są odrzucane
	    {
	      c_pos(24, 18); cout << "        ";
	      c_col(A_BRIGHT, C_YELLOW, C_BLUE);
	      c_pos(24, 19);
	      yesno = getch(1);
	      if(yesno == 'T' || yesno == 't') { player = true;  next = true; } else
	      if(yesno == 'N' || yesno == 'n') { player = false; next = true; } else C_BELL;
	    }
	}

      c_clr(0);
      c_pos(0, 0);
      c_col(A_BRIGHT, C_WHITE, C_BLUE);
      draw_table(1); //narysuj planszę z cyframi

      if(player == false) //jeśli najpierw gra cpu
    	{
	  for(int i = 1; i < 4 || again == true ; i++) //8 taktów
	    {
	      ai_cpu(1); ticks++; rules();
	      ai_plr();  ticks++; rules();
	    }
	  ai_cpu(1); ticks++; rules(); //ostatni takt gry
	}

      else

      if(player == true) //jeśli najpierw gra plr
    	{
	  for(int i = 1; i < 4 || again == false ; i++) //8 taktów
	    {
	      ai_plr();  ticks++; rules();
	      ai_cpu(1); ticks++; rules();
	    }
	  ai_plr(); ticks++; rules(); //ostatni takt gry
    	}
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
