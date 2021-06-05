//Henter in pn523 bibliotek
#include <pn532.h>
#include <pn532_uno.h>
#include "Variabler.h"

//Til NFC scanner
uint8_t buff[255];
uint8_t uid[MIFARE_UID_MAX_LENGTH];
int32_t uid_len = 0;
PN532 pn532;

void setup() {
  // Setter opp NFC scanner
  PN532_SPI_Init(&pn532);
  settAlleLister();   //Resetter alle lister, slik at det står false på alt
  Serial.println("Hello!");
  if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK) {
    Serial.print("Found PN532 with firmware version: ");
    //Serial.print(buff[1], DEC);
    Serial.print(".");
    //Serial.println(buff[2], DEC);
    Serial.println("Waiting for RFID/NFC card...");
  }
  PN532_SamConfiguration(&pn532);

  //Åpner boardet med morsom lys sekvens
  for (int g=0; g<8; g++){
    for (int h=0; h<4; h++){
      alleDagerLys[g][h].off();
    }
  }
  for (int y=0; y<10; y++){
    alleAktLys[y].off();
  }
  
  for (int y=0; y<10; y++){
    alleAktLys[y].on();
    delay(80);
  }
  
  for (int g=0; g<8; g++){
    for (int h=0; h<4; h++){
      alleDagerLys[g][h].on();
      delay(60);
    }
  }
  
  delay(2000);
}

void loop() {
  // Setter NFC scanner til å lete etter en brikke
  //Serial.println("loop");
  uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 100);
  
  if (uid_len == PN532_STATUS_ERROR){     //Hvis ingenting blir lest
    //Serial.println("nada lest");
    //Finner verdiene fra potentometeret:
    int verdi = map(analogRead(potPin), 0, 1023, 6, 0);    //potentometer har når kunn 7 verdier, 0-6      //Neste prototype: 0-13 - 2 uker
    aktivDag = verdi;   //Potetntometere bestemmer hvilken dag som er aktiv
    //Serial.println("HER");
    if (teller < 50){   //Vi har en teller som gjør at aktiv dag vil slutte å lyse etter hvert om ingenting blir gjort
      visAktivDag(aktivDag);    //Lyser for aktiv dag
    } 
    //Serial.println("Pot");
    //Serial.println(verdi);
    teller++;   //Legger til en på telleren
    if (sjekkPot()){    //Hvis potentometeren blir flyttet på
      teller = 0;   //Setter telleren til 0, slik at tiden blir nullet ut til ny dag
    }
  } 
  else {    // Personkort lest  
    //If test som sjekker at det ikke er aktivitetskort aom blir scannet
    if( sammenlign(uid, brikkeBlaa) || sammenlign(uid, brikkeRoed) || sammenlign(uid, brikkeGul) || sammenlign(uid, brikkeGroenn) ){
      lest();   //Signaliserer til bruker at brikke er lest
      int sjekk = sjekkPersonKort(uid);   //Finner hvilken person brikken hører til
      if (aktivitetFinnes(aktivDag)){       //Om det er registrert en altivitet på den aktive dagen
        settDelta(sjekk, aktivDag);   //setter man at person vil delta
      }
      else{   //Hvis det ikke er aktivitet på den dagen
        leggTilAktivitet(sjekk, aktivDag);    //Må personen legge til en aktivitet
      } 
      teller = 0;   //etter det går teller til null
      oppdateringOppmote();   //Oppdaterer oppmøte
    } 
  }
}

void lest(){
  //Serial.println("lest");
  //Får aktivitetslysene/topp lysene til å blinke en gang
  nullStillAktivitet();   //Skrur av aktivitet lys
  for (int i=0; i<10; i++){
    alleAktLys[i].on();
  }
  delay(300);
  for (int j=0; j<10; j++){
    alleAktLys[j].off();
  }
}

bool aktivitetFinnes(int aktDag){
  //Går igjennom alle aktivitetstillstandene for aktiv dag
  for (int i=0; i<10; i++){ 
    if (alleAktTilstander[aktDag][i] == true){    //HVis en av tillsandene er true
      return true;    //Returnere true fordi da finnes det en aktivitet
    } 
  }
  return false; //Hvis ikke den returnerer tru inne i for-løkken returnerer false
}

void settDelta(int personTall, int dag){
  //Setter deltgelse i oppmoteListe 
  oppmoteListe[dag][personTall-1] = true;
}

void leggTilAktivitet(int person, int dagen){
  //Serial.println("start");
  //Delay 2 sekunder for at NFC scanneren ikke skal scanne brikken igjenn
  delay(2000);

  //NFC scanner venter på et aktivitetskort
  uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 10000);
  int leggTilAkt = finnAktivitet(uid);    //finner riktig aktivitet nummer etter hvilket kort som ble scannet
  alleAktTilstander[dagen][leggTilAkt-1] = true;  //Setter tillstanden til aktiviteten
  alleDagerLys[dagen][person-1].on();   //Aktivitets lyset som samhandler med kortet lyser
  settDelta(person, dagen);   //Setter at personen som la til aktiviteten vil delta
  
}


boolean sammenlign(uint8_t *brikke, uint8_t *fasit) {
  //Sammenligner to brikker
  for(int n=0;n<4;n++) if (brikke[n] !=fasit[n]) return false;

  return true;
}

void nullStillAktivitet(){
  //Skrur av alle aktivitetslys
  for (int i=0; i<10; i++){
      alleAktLys[i].off();
    }
}

void visAktivDag( int dagNr){
  //Skrur av alle dag lys
  for (int j=0; j<4; j++){
    if (oppmoteListe[dagNr][j] == false){
      alleDagerLys[aktivDag][j].off();
    }

  }
  
  nullStillAktivitet();   //Skrur av aktivitet lys

  if (aktivitetFinnes(dagNr)){        //Hvis aktivitet finnes
     for (int k=0; k<10; k++){      
      //Går igjennom alle aktivitetstilstandene på den dagen
      if (alleAktTilstander[dagNr][k] == true){   //Hvis en av dem er true:
        alleAktLys[k].on();     //Viser aktivitet lyset
      }
     }  
   }

  Serial.println("Viser aktiv dag");
  
  //Blinker på dagen du er
  for (int k=0; k<4; k++){
    if(oppmoteListe[dagNr][k] == false){
      alleDagerLys[dagNr][k].on();
    }
  }
  delay(250);
  for (int k=0; k<4; k++){
    if(oppmoteListe[dagNr][k] == false){
      alleDagerLys[dagNr][k].off();
    }
  }
  delay(250);

  //Oppdatereer oppmøte
  oppdateringOppmote();
  
}

void oppdateringOppmote() {
  //Skur av alle lys
  for(int i = 0; i < 8; i++) {
    for(int y = 0; y < 4; y++) {
      alleDagerLys[i][y].off();
    }
  }
  
  //Setter på lys som korrensponderer med oppmøte og personer som skal møte opp på hvilke dag
  for(int i = 0; i < 8; i++) {
    for(int y = 0; y < 4; y++) {
      if(oppmoteListe[i][y] == true) {
        alleDagerLys[i][y].on();
      }
    }
  }
}

void settFalse() {
  //En type reset, setter alt til false
  for(int i = 0; i < 8; i++) {
    for(int y = 0; y < 4; y++) {
      oppmoteListe[i][y] = false;
    }
  }
  for(int i = 0; i < 8; i++) {
    for(int y = 0; y < 10; y++) {
      alleAktTilstander[i][y] = false;
    } 
  }
}

void settAlleLister() {
  settFalse();
}

void settPaalogget(int i) {
  //Setter hvilken bruker som er pålogget
  paalogget = i;
}

int sjekkPersonKort(uint8_t *uid) {
  //Sjekker det som ble scannet uid, med alle personbrikkene for å bestemme hvilken person som er logget på

  //Rød
  if(sammenlign(uid, brikkeRoed) == true) {
    settPaalogget(1);
    return 1;
  }
  //Blå
  else if(sammenlign(uid, brikkeBlaa) == true) {
    settPaalogget(2);
    return 2;
  }
  //Gul
  else if(sammenlign(uid, brikkeGul) == true) {
    settPaalogget(3);
    return 3;
  }
  //Grønn
  else if(sammenlign(uid, brikkeGroenn) == true) {
    settPaalogget(4);
    return 4;
  }
}

int finnAktivitet(uint8_t *uid) {
  lest();   //Viser til bruker at et aktivitetskort er lest
  Serial.print("Lest akt");
  //Sammenligner uid, med alle aktivitetene får å finne hvilket som ble scannet
  if(sammenlign(uid, lageMiddag) == true) {
    //ledMiddag.on();
    //alleAktTilstander[aktivDag][0] = true;
    Serial.print("la til 1");
    return 1;
  }
  else if(sammenlign(uid, piknikk) == true) {
    //ledPiknikk.on();
    //alleAktTilstander[aktivDag][1] = true;
    Serial.print("la til 2");
    return 2;
  }
  else if(sammenlign(uid, trene) == true) {
    //ledTrene.on();
    //alleAktTilstander[aktivDag][2] = true;
    Serial.print("la til 3");
    return 3;
  }
  else if(sammenlign(uid, tV) == true) {
    //ledTv.on();
    //alleAktTilstander[aktivDag][3] = true;
    Serial.print("la til 4");
    return 4;
  }
  else if(sammenlign(uid, kaffeUte) == true) {
    //ledKaffe.on();
    //alleAktTilstander[aktivDag][4] = true;
    Serial.print("la til 5");
    return 5;
  }
  else if(sammenlign(uid, vinkveld) == true) {
    //ledVin.on();
    //alleAktTilstander[aktivDag][5] = true;
    Serial.print("la til 6");
    return 6;
  }
  else if(sammenlign(uid, spa) == true) {
    //ledSpa.on();
    //alleAktTilstander[aktivDag][6] = true;
    Serial.print("la til 7");
    return 7;
  }
  else if(sammenlign(uid, rydde) == true) {
    //ledRydde.on();
    //alleAktTilstander[aktivDag][7] = true;
    Serial.print("la til 8");
    return 8;
  }
  else if(sammenlign(uid, shopping) == true) {
    //ledShopping.on();
    //alleAktTilstander[aktivDag][8] = true;
    Serial.print("la til 9");
    return 9;
  }
  else if(sammenlign(uid, tur) == true) {
    //ledTur.on();
    //alleAktTilstander[aktivDag][9] = true;
    Serial.print("la til 10");
    return 10;
  }
  else{
    Serial.print("la til 20");
    return 20;
  }
}


bool sjekkPot(){
   //Lese potentometer først; (to ganger)
  naaVerdi = analogRead(potPin);
  naaVerdi = analogRead(potPin);

  naaVerdi = constrain(naaVerdi, 8, 1015);    //Ha vekk verdiene på starten og enden   

  if (naaVerdi < (gammelVerdi - 4) || naaVerdi > (gammelVerdi + 4)) {   //Hvis naa verdi er endret
    gammelVerdi = naaVerdi;   //Sett gammelverdi til naa verid
    // overfører til prosent
    potPros = map(gammelVerdi, 8, 1008, 0, 100);    //0% - 100%
    //Skjekke om prosent endres
    if (gammelPros != potPros) {
      gammelPros = potPros;   //Setter gammelprosent til prosent endring fra potentometeret
    }
    return true;    //Returnerer true, siden den ble rotert
  }
  return false; //HVis ikke rotert: return false
}
/*
//lytte etter person kort?
    uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
    int sjekk = sjekkPersonKort(uid); //Paalogget

    //Hvis det finnes en aktivitet så legges til personen på oppmøte
    
    if (aktivitetFinnes(dagNr)){
      oppmoteListe[dagNr][sjekk-1] = true;
    } else {
      leggTilAktivitet(sjekk, dagNr);       //I den metoden legges til og deltakelst
       //Hvis aktivitet ikke finnes alt
      // aktivitets kort
      for (int i=0; i<10; i++){
        if (alleAktTilstander[dagNr][i] == true){
          alleAktLys[i].on();   //Setter på det nye lyset.
        }
      }
    }

    
*/
