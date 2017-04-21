#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    // to tylko wskaźniki, nie potrafią przechowywać tekstu.
    // mogą przechowywać **tylko** adresy (np do pamięci z tekstem ;)
    char *nick;
    char *pass;
} konto;

int main(int argc, char **argv)
{
    // zmienna na stosie
    konto mati;

    // to też zmienna na stosie, ale jako wskaźnik!
    konto *seba;

    // --- stos
    // zmienna `mati` istnieje do końca funkcji main i jest
    // automatycznie usuwana za nas.
    // mati.nick to pamięć zaalokowana ręcznie czyli teoretycznie
    // istnieje nadal po funkcji main (w rzeczywistości po funkcji
    // main kończy się program i system operacyjny sprząta po nas
    // pamięć, ale możemy założyć, że jeśli jej nie zwolnimy, to
    // nadal jest zaalokowana).
    mati.nick = malloc(16);
    mati.pass = malloc(16);

    // wypisze sizeof(ptr_t * 2) - bo struktura konto ma tylko dwa
    // pola, nick i pass. Oba pola to **wskaźniki** (nie stringi)
    // które mają stały rozmiar 4 bajtów (albo 8 zależy od architektury).
    printf("sizeof(mati) = %d\n", sizeof(mati));

    // kopiuję string do zaalokowanej pamięci.
    // mógłbym po prostu przypisać mati.nick = "mati" ale wtedy
    // straciłbym możliwość modyfikacji tego tekstu (bo mati.nick
    // to wskaźnik który po prostym przypisaniu wskazywałby na
    // string w sekcji pamięci .rodata. Tutaj musiałbyś poczytać o tym,
    // gdzie żyją różne typy zmiennych.
    strcpy(mati.nick, "mati");
    strcpy(mati.pass, "itam");

    // --- sterta
    // seba to wskaźnik który 'dostaje' nową pamięć od systemu.
    // zauważ, że pamięć z malloca zostanie dopóki nie zwolnimy
    // jej ręcznie, ale zmienna seba mimo, że jest wskaźnikiem i tak
    // zniknie po funkcji main. To jest bardzo ważny element
    // zarządzania pamięcią - ręczna alokacja to nie jest jakiś
    // inny, niezwykły typ pamięci - to po prostu **adres** gdzieś
    // w pamięci RAM do którego system pozwoli nam wpisywać i odczytywać
    // dane.
    // jedyna różnica jest taka, że mati jest na stosie, zmienne
    // mati.nick i mati.pass też są na stosie ale wskazują
    // na pamięć na stercie.
    // natomiast seba to wskaźnik który wskazuje na stertę (gdzie
    // znajduje się struktura wraz ze zmiennymi seba->nick i seba->pass)
    // które też wskazują na inne miejsca w stercie.
    seba = malloc(sizeof(*seba));
    seba->nick = malloc(16);
    seba->pass = malloc(16);

    // wypisze rozmiar typu seba, czyli **wskaźnika** na strukturę.
    // seba to wskaźnik, a nie cała struktura!
    // wskaźnik trzyma adres (pamięci z linii 56) gdzie leży cała faktyczna
    // struktura wraz ze zmiennymi seba->nick i seba->pass.
    printf("sizeof(seba) = %d\n", sizeof(seba));

    // znów kopiowanie, nic ciekawego.
    strcpy(seba->nick, "seba");
    strcpy(seba->pass, "abes");

    // mati to zmienna lokalna, struktura która jest `tutaj` blisko na stosie,
    // więc odwołanie się do jej zmiennych jest bezpośrednie.
    printf("stos\nnick: %s\npass: %s\n\n", mati.nick, mati.pass);

    // seba to **wskaźnik** do struktury która leży gdzieś hen hen daleko.
    // trzeba odczytać **adres** który trzyma ten wskaźnik, czyli użyć dereferencji.
    // zapis cos->cos to tak naprawdę zapis (*cos).cos
    // *cos to dereferencja wskaźnika, czyli zajrzenie pod adres na który wskazuje.
    printf("sterta\nnick: %s\npass: %s\n", seba->nick, seba->pass);


    // --- zwalnianie pamięci
    // mati to zmienna na stosie, więc sama się usunie **ale** zawiera dwa wskaźniki
    // które wskazują na pamięć alokowaną ręcznie, która już się sama **nie** usunie.
    // dlatego robimy to ręcznie.
    free(mati.nick);
    free(mati.pass);

    // kolejna ważna rzecz - zmienna seba też jest na stosie i też się sama usunie.
    // ale zmienna seba to **tylko** wskaźnik, a nie struktura którą alokowaliśmy.
    // tutaj więc jest jeszcze więcej roboty, bo musimy dodatkowo zwolnić całą strukturę
    // zaraz po tym, jak usuniemy jej pola.
    free(seba->nick);
    free(seba->pass);
    free(seba); // << ten dodatkowy krok
}
