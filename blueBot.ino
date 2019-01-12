/*-----------------------------------------------------------------------------------------------------
 * Produit par:
 * Chloé Champoux & Maxime Tremblay
 * EDM-4640 UQAM 2017
 * 
 * DESCRIPTIF:
 *  Déplacement d'une machine simple basé sur l'analyse de la luminosité par photorésistances
 * 
 * CONTRAINTE:
 * Comportement de peur relié à la luminosité (le sujet fuit la lumière)
 ------------------------------------------------------------------------------------------------------*/
#include <Chrono.h>
#include <math.h>
Chrono messageSendInterval = Chrono();

// ICLUDE MASSAGE PACKER
#include <AsciiMassagePacker.h>

// PACKER(FOR SENDING) INSTANCE:
AsciiMassagePacker outbound;
/*Constantes reliées aux moteurs de gauche et de droite*/
/*-Moteur de Droit*/
const byte droitIN1 = 7;
const byte droitIN2 = 8;
const byte PWMdroit = 9;

/*-Moteur de Gauche*/
const byte gaucheIN1 = 3; 
const byte gaucheIN2 = 4; 
const byte PWMgauche = 5;

/*BROCHES DE LA LED 3 COULEURS*/
const int rouge = 10;
const int vert = 11;
const int bleu = 12;

/*Variables en mémoire de la source du danger (la luminosité)*/
int sourceDanger = 0;

/*Photoresistances*/
int oeil_photocell_Gauche;
int oeil_photocell_Droit;

/*Seuils de la valeur de luminosité*/
int seuil_lum = 600;

/*vitesse de déplacement de base*/
int vitBase = 120;

/*vitesses minimum et maximale pour le deplacement en exploration*/
int vitMin = 60;
int vitMax = 150;

/*Vitesses de base des moteurs pour le mode exploration*/
int vitBaseDroit = vitBase;
int vitBaseGauche = vitBase;

/*************************************************************************************/
void setup()
{
  /*Débogage USB************/
     Serial.begin(57600);
  /*************************/
  
  /*Moteur 1 - Droit*/
  pinMode(droitIN1, OUTPUT);
  pinMode(droitIN2, OUTPUT);
  pinMode(PWMdroit, OUTPUT);
  
  /*Moteur 2 - Gauche*/
  pinMode(gaucheIN1, OUTPUT);
  pinMode(gaucheIN2, OUTPUT);
  pinMode(PWMgauche, OUTPUT);

  
}

void loop() 
{
  /*Valeurs de luminosité de chaques cotés en temps réel*/
  oeil_photocell_Gauche = analogRead( A1 );
  oeil_photocell_Droit = analogRead( A0 );
  
  /*Analyse du niveau de luminosité*/
  verifierLuminosite(oeil_photocell_Gauche, oeil_photocell_Droit);
  
  /*Déplacement du sujet dans l'espace*/
  deplacement(oeil_photocell_Gauche, oeil_photocell_Droit);

  
}

/*Fonction pour l'analyse d'une source lumineuse et de la position dans l'espace*/
void verifierLuminosite(int val_lumGauche, int val_lumDroit){

      /*Si une source stimule les 2 photoresistances en même temps*/
      if(val_lumGauche >= seuil_lum && val_lumDroit >= seuil_lum){
          /*Estimation de la source vers l'avant*/
          sourceDanger = 1;
      }
      else{
     
            /*Si une source stimule la photoresistance de gauche*/
            if(val_lumGauche >= seuil_lum && val_lumDroit < seuil_lum){
              /*Estimation de la source vers la gauche*/
                sourceDanger = 2;
              }
              
            /*Si une source stimule la photoresistance de droite*/
            if(val_lumDroit >= seuil_lum && val_lumGauche < seuil_lum){
              /*Estimation de la source vers la droite*/
                sourceDanger = 3;
              }
              
            /*Si aucune source stimule les photoresistances*/
            if(val_lumGauche < seuil_lum && val_lumDroit < seuil_lum){
              /*Estimation d'aucune source*/
                sourceDanger = 0;
              }
      } 
}

/*Fonction pour le déplacement dans l'espace selon l'analyse du comportement basé sur la source du danger(la luminosité)*/
void deplacement(int val_lumGauche, int val_lumDroit){
        /*Variables de vitesse pour chaques cotés selon la luminosité*/
            int vitesseDroit = ceil(map(val_lumDroit, 0, 1023, vitBase, 255));
            int vitesseGauche = ceil(map(val_lumGauche, 0, 1023, vitBase, 255));
      /*Gestion de la source de danger*/
      switch(sourceDanger){
        
          /*Si la source lumineuse provient de l'avant, faire reculer le sujet*/
        case 1 :
            /*Envoie de la vitesse aux moteurs respectifs*/
            setMoteur(-vitesseDroit, droitIN1, droitIN2, PWMdroit);
            setMoteur(-vitesseGauche, gaucheIN1, gaucheIN2, PWMgauche);
            digitalWrite(rouge, HIGH);
            digitalWrite(vert, LOW);
            digitalWrite(bleu, LOW);
        break;
          /*Si la source lumineuse provient de la gauche, faire tourner le sujet à droite*/
        case 2 :
            /*Envoie de la vitesse aux moteurs respectifs*/
            setMoteur(vitMin, droitIN1, droitIN2, PWMdroit);
            setMoteur(vitesseGauche, gaucheIN1, gaucheIN2, PWMgauche);
            digitalWrite(vert, HIGH);
            digitalWrite(rouge, LOW);
            digitalWrite(bleu, LOW);
        break;
          /*Si la source lumineuse provient de la droite, faire tourner le sujet à gauche*/
        case 3 :
        /*Envoie de la vitesse aux moteurs respectifs*/
            setMoteur(vitesseDroit, droitIN1, droitIN2, PWMdroit);
            setMoteur(vitMin, gaucheIN1, gaucheIN2, PWMgauche);
            digitalWrite(bleu, HIGH);
            digitalWrite(rouge, LOW);
            digitalWrite(vert, LOW);
        break;
        
          /*Si aucune source lumineuse est détectée, passer en mode exploration*/
        case 0 :
            explorer();
        break;

      }
}

/*Fonction d'exploration libre dans l'espace*/
void explorer(){
  
  /*Variation de l'aléatoire*/
  randomSeed(analogRead(0));
  
  /*Variables de vitesse aléatoire pour chaques cotés, arrondies en nombre entier*/
  int vitRandomDroit = (int)random(vitMin, vitMax);
  int vitRandomGauche = (int)random(vitMin, vitMax);

  /*Envoie des valeurs de vitesses aléatoires aux moteurs*/
  setMoteur(vitRandomDroit, droitIN1, droitIN2, PWMdroit);
  setMoteur(vitRandomGauche, gaucheIN1, gaucheIN2, PWMgauche);

  /*DÉSACTIVER LES LUMIÈRES*/
  digitalWrite(rouge, LOW);
  digitalWrite(vert, LOW);
  digitalWrite(bleu, LOW);
 
}

/*Fonction pour la gestion d'un moteur selon la vitesse passée en paramètre*/
void setMoteur(int motorSpeed, byte IN1, byte IN2, byte PWM){
      /*Si la vitesse est positive, conserver la charge magnétique du moteur pour avancer*/
      if (motorSpeed > 0)
      {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      }
      /*Si la vitesse est négative, inverser la charge magnétique du moteur pour reculer*/
      else if (motorSpeed < 0)
      {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      /*Si la vitesse est de 0, définir une charge magnétique identique pour arrêter le moteur*/
      else 
      {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
      }
      /*Envoyer la valeur de vitesse absolue au moteur*/
//      digitalWrite(PWM, HIGH);
analogWrite(PWM, abs(motorSpeed));
      /*-----------------------------------------------------------*/
  /*MSG POUR DEBUG USB---SERIAL
   if ( messageSendInterval.hasPassed(100) ) {
                messageSendInterval.restart();
                outbound.beginPacket("vitesse");
                outbound.addInt( motorSpeed );
                outbound.endPacket();
                Serial.write(outbound.buffer(), outbound.size());
   }
   «
  /*------------------------------------------------------------*/
}
