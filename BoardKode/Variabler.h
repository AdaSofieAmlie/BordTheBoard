#include "Led.h"

// Pålogget bruker. Settes til 0 etter tid
int paalogget = 0;
// aktiv dag (0-13(6 i PT))
int aktivDag = 0;

int teller = 0;

//For å ha true/false om potentometeren roterer
int potPin = A0;
int gammelVerdi;
int naaVerdi;
byte potPros;
byte gammelPros;

//Setter opp alle brukere: listene er RFID til hver brikke og kort
// Bruker1
uint8_t brikkeRoed[10] = {164, 58, 46, 49, 0, 0, 0, 0, 0, 0};
const int bruker1 = 1;

// Bruker2
uint8_t brikkeBlaa[10] = {98, 239, 218, 27, 0, 0, 0, 0, 0, 0};
const int bruker2 = 2;

// Bruker3
uint8_t brikkeGul[10] = {34, 182, 245, 27, 0, 0, 0, 0, 0, 0};
const int bruker3 = 3;

// Bruker4
uint8_t brikkeGroenn[10] = {228, 193, 180, 87, 0, 0, 0, 0, 0, 0};
const int bruker4 = 4;

//AktiviteterUID
uint8_t lageMiddag[10] = {103, 116, 35, 217, 0, 0, 0, 0, 0, 0};
uint8_t piknikk[10] = {07, 235, 34, 217, 0, 0, 0, 0, 0, 0};
uint8_t trene[10] = {247, 60, 251, 215, 0, 0, 0, 0, 0, 0};
uint8_t tV[10] = {135, 156, 244, 215, 0, 0, 0, 0, 0, 0};
uint8_t kaffeUte[10] = {151, 23, 245, 215, 0, 0, 0, 0, 0, 0};
uint8_t vinkveld[10] = {215, 232, 17, 217, 0, 0, 0, 0, 0, 0};
uint8_t spa[10] = {151, 225, 243, 215, 0, 0, 0, 0, 0, 0};
uint8_t rydde[10] = {135, 36, 246, 215, 0, 0, 0, 0, 0, 0};
uint8_t shopping[10] = {151, 62, 246, 215, 0, 0, 0, 0, 0, 0};
uint8_t tur[10] = {71, 176, 246, 215, 0, 0, 0, 0, 0, 0};

//Aktivitetslys
Led ledMiddag(46);
Led ledPiknikk(41);
Led ledTrene(36);
Led ledTv(31);
Led ledKaffe(23);
Led ledVin(26);
Led ledSpa(22);
Led ledRydde(2);
Led ledShopping(12);
Led ledTur(13);


//PersonLys per dag (Sender inn pins til objekter av Led)

//man1
Led man1B1(53);
Led man1B2(49);
Led man1B3(48);
Led man1B4(47);

int moteM1[4] = {false, false, false, false};
bool lysM1[10] = {false, false, false, false, false, false, false, false, false, false};
//tir1
Led tir1B1(45);
Led tir1B2(44);
Led tir1B3(43);
Led tir1B4(42);

int moteT1[4] = {false, false, false, false};
bool lysT1[10] = {false, false, false, false, false, false, false, false, false, false};
//ons1
Led ons1B1(40);
Led ons1B2(39);
Led ons1B3(38);
Led ons1B4(37);

int moteO1[4] = {false, false, false, false};
bool lysO1[10] = {false, false, false, false, false, false, false, false, false, false};
//tor1
Led tor1B1(35);
Led tor1B2(34);
Led tor1B3(33);
Led tor1B4(32);

int moteTO1[4] = {false, false, false, false};
bool lysTO1[10] = {false, false, false, false, false, false, false, false, false, false};
//fre1
Led fre1B1(28);
Led fre1B2(27);
Led fre1B3(29);
Led fre1B4(30);

int moteF1[4] = {false, false, false, false};
bool lysF1[10] = {false, false, false, false, false, false, false, false, false, false};
//lør1
Led lor1B1(3);
Led lor1B2(5);
Led lor1B3(6);
Led lor1B4(7);

int moteL1[4] = {false, false, false, false};
bool lysL1[10] = {false, false, false, false, false, false, false, false, false, false};
//søn1
Led son1B1(8);
Led son1B2(9);
Led son1B3(10);
Led son1B4(11);

int moteS1[4] = {false, false, false, false};
bool lysS1[10] = {false, false, false, false, false, false, false, false, false, false};
//man2
Led man2B1(18);
Led man2B2(17);
Led man2B3(16);
Led man2B4(15);

int moteM2[4] = {false, false, false, false};
bool lysM2[10] = {false, false, false, false, false, false, false, false, false, false};
//tir2
//ons2
//tor2
//fre2
//lør2
//søn2

// Alle personer per dag lys
Led alleDagerLys[8][4] = {{man1B1, man1B2, man1B3, man1B4}, {tir1B1, tir1B2, tir1B3, tir1B4}, {ons1B1, ons1B2, ons1B3, ons1B4}, 
{tor1B1, tor1B2, tor1B3, tor1B4}, {fre1B1, fre1B2, fre1B3, fre1B4}, {lor1B1, lor1B2, lor1B3, lor1B4}, {son1B1, son1B2, son1B3, son1B4},
{man2B1, man2B2, man2B3, man2B4}};

// Alle aktivitetslys
Led alleAktLys[10] = {ledMiddag, ledPiknikk, ledTrene, ledTv, ledKaffe, ledVin, ledSpa, ledRydde, ledShopping, ledTur};
//bool aktLysPaa[10] = {false, false, false, false, false, false, false, false, false, false};

//Liste over alle listene over oppmøte
bool oppmoteListe[8][4] =  {{moteM1}, {moteT1}, {moteO1}, {moteTO1}, {moteF1}, {moteL1}, {moteS1}, {moteM2}}; 

//Liste oer alle aktivitetstilstandene i de forskjellige dagene
bool alleAktTilstander[8][10] = {{lysM1}, {lysT1}, {lysO1}, {lysTO1}, {lysF1}, {lysL1}, {lysS1}, {lysM2}};
