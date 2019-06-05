/* 
 *  Eclairage LED - Couloir Batiment A1
 *  
 *  Version : 19.6.4 (Sans système d'interruption)
 *  Auteurs : Cavé Jeremy, Dutrouilh Ludovic, Lacoste Malcolm | L2-EEA 2019
 *  
 */



/* 
 *  Paramètres 
 */

#include <stdbool.h>
#define nombre_de_strips 7

int inh_a = 12, inh_b = 11;
int adressA = 9, adressB = 8, adressC = 7;
int pwmOUT = 10, sortieSwitch = 5;

struct STRIP{int DMUX,G,R,B;};
struct STRIP couloirs[nombre_de_strips]={ {0,0,1,2}, {0,3,4,5}, {1,0,1,2}, {1,3,4,5}, {2,0,1,2}, {2,3,4,5}, {3,0,1,2}};

/* Pins ANALOG de l'Arduino des détecteurs de mouvement */
int move_detector[1] = {0};
int valeur_mini_detection = 550; /* Mini : 000, Max : 750 */

/* Réglages des délais */
int delai_transition = 5000, //Transition entre chenillard et ambiance, en millisecondes.
    delai_entre_detection = 250, //Toutes les n ms, vérifie si quelqu'un est là.
    delai_marche = 285; //En ms, décalage entre chaque allumage de strips lors de la fonction "escalier()"

/* Contrôle la loop */
int presence = false;



/*
 *  Début du programme
 */
 
void setup() {
    pinMode(inh_a, OUTPUT); pinMode(inh_b, OUTPUT);
    pinMode(adressA, OUTPUT); pinMode(adressB, OUTPUT); pinMode(adressC, OUTPUT);
    pinMode(pwmOUT, OUTPUT); pinMode(sortieSwitch, OUTPUT);
    pinMode(move_detector[0], INPUT);
    Serial.begin(9600);
    delay(5000); //Setup des capteurs de mouvement
}

void loop() {
    if(analogRead(move_detector[0]) > valeur_mini_detection){
        if(presence != true){
            Serial.println("Déclenchement.");
            escalier();
            presence = true;}
        ambiance_3e_etage();}
    presence = false;
    Serial.println("Extinction.");
    delay(delai_entre_detection);
}



/*
 *  Fonctions utiles
 */
 
void escalier(){
/* Allumage bout-à-bout, utilisé à la détection d'un mouvement */
    int t0;
    for(int strip=0; strip < nombre_de_strips; strip++){
        t0=millis();
        while((millis()-t0) < delai_marche){
            for(int allume=0; allume <= strip; allume++){
                lumiere_blanche(allume);}}}
        t0=millis();
    while((millis()-t0) < delai_transition){
        for(int strip=0; strip < nombre_de_strips; strip++){
            lumiere_blanche(strip);}}
}

void positionnement_dmux(int strip){
/* Choix du DMUX correspondant vers la bande */
    digitalWrite(inh_a, bitRead(couloirs[strip].DMUX,0));
    digitalWrite(inh_b, bitRead(couloirs[strip].DMUX,1));
}

void positionnement_grb(int strip, bool GREEN, bool RED, bool BLUE){
/* Adressage A,B,C correspondant vers la bande */
    if(GREEN){
        digitalWrite(adressA, bitRead(couloirs[strip].G, 0)); digitalWrite(adressB, bitRead(couloirs[strip].G, 1)); digitalWrite(adressC, bitRead(couloirs[strip].G, 2));};
    if(RED){
        digitalWrite(adressA, bitRead(couloirs[strip].R, 0)); digitalWrite(adressB, bitRead(couloirs[strip].R, 1)); digitalWrite(adressC, bitRead(couloirs[strip].R, 2));};
    if(BLUE){
        digitalWrite(adressA, bitRead(couloirs[strip].B, 0)); digitalWrite(adressB, bitRead(couloirs[strip].B, 1)); digitalWrite(adressC, bitRead(couloirs[strip].B, 2));};
}

void allumage(int haut, int bas){
/* Recréation d'un OUTPUT PWM personnalisé pour palier au delta situé à l'état bas, l'état bas devient donc " superposable " avec les autres */
    digitalWrite(pwmOUT, HIGH);
    delayMicroseconds(haut);
    digitalWrite(pwmOUT, LOW);
    delayMicroseconds(bas);
}

void lumiere_blanche(int strip){
/* Configuration RGB au niveau maximum */
    int etat_haut=500, etat_bas=0; // ~650Hz -> (500µs+0)*3
    positionnement_dmux(strip); positionnement_grb(strip, true, false, false); allumage(etat_haut, etat_bas);
    positionnement_dmux(strip); positionnement_grb(strip, false, true, false); allumage(etat_haut, etat_bas);
    positionnement_dmux(strip); positionnement_grb(strip, false, false, true); allumage(etat_haut, etat_bas);
}
void ambiance_3e_etage(){
/* ~7sec de délai par "timer" avec etat_bas=450, etat_haut=50 */
    int etat_haut, etat_bas;
    for(int timer=1; timer<=1; timer++){
        for(etat_bas=450, etat_haut=50; etat_bas>50; etat_bas--, etat_haut++){
            for(int i=0; i <= nombre_de_strips; i++){
                    positionnement_dmux(i); positionnement_grb(i, false, false, true); allumage(etat_bas, etat_haut);}
            for(int i=0; i <= nombre_de_strips; i++){                
                    positionnement_dmux(i); positionnement_grb(i, true, false, false); allumage(etat_haut, etat_bas);}}
        for(etat_bas=450, etat_haut=50; etat_bas>50; etat_bas--, etat_haut++){
            for(int i=0; i <= nombre_de_strips; i++){
                    positionnement_dmux(i); positionnement_grb(i, true, false, false); allumage(etat_bas, etat_haut);}
            for(int i=0; i <= nombre_de_strips; i++){                
                    positionnement_dmux(i); positionnement_grb(i, false, false, true); allumage(etat_haut, etat_bas);}}}
}
