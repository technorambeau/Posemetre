//############################################################################################################################################################################
    // DATE DE MISE A JOURS
      char DateMiseAJours[]  = "14/10/16";   // 15H30
      char VersionProgramme[]  = "V2.0";     //  Publication sur Git, passage a version 2.0
      
    // librairies Ecran I2C 0.96' 128*64
      #include <Wire.h>                      // 128*64 On dispose de 8 lignes de 21 caractères avec la fonte ASCII-standard 5x7  
      #include <Adafruit_GFX.h>              // colonne 6px par caractere
      #include <Adafruit_SSD1306.h>          // ligne 8px par ligne
      #define OLED_RESET 4
      Adafruit_SSD1306 display(OLED_RESET);

    // librairies LuxMetre
      #include <BH1750FVI.h>
      BH1750FVI LightSensor;

    // librairies gestion de memoir (permet de stoker les constantes dans l'EEPROM)
      #include <avr/pgmspace.h>

    // Broches numériques
      const int BPmode = 2;                  // Bouton mode sur broche 2
      const int BP_moins = 6;                // Bouton - sur broche 6
      const int BP_plus = 7;                 // Bouton + sur broche 7
      
    // Temps entre deux affichage sur l'ecran.
      int DelayRafraichissement = 50;
      
// GESTION MODES
      char* tableauModeAffichage[] = {"ISO","OUVERTURE","VITESSE", "STENOPE"}; // mode pour affichage ecran
      byte NombreDeMode = 4;
      byte MODE = 0;                         // variable pour le comptage du nombre d'appuis sur le bouton poussoir
      byte EtatBPmode = 0;                   // Variable pour l'état actuel du bouton poussoir
      byte lastEtatBPmode = 0;               // Variable pour l'état précédent du bouton poussoir

// GESTION LUMIERE
      char tableauIL[] =                  {0,  1, 2,   3, 4,   5,  6,   7,   8,    9,   10,   11,    12,    13,    14,    15,    16,     17,      18};
      float tableauEquivalenceLuxIL [] = {2.5, 5, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 163840, 3276680, 655360};
      byte  nombreDeValeursIL = 19;
      float LUX = 0;                           // Lux 
      char IL = 0;                           // Convertion des Lux en IL
      char ILs = 0;                          // Convertion des IL en ILs en fonction de l'ISO

// AFFICHAGE MODE STENOPE
      int heures=0;
      int minutes=0;
      int secondes=0;

// GESTION ISO (sensibilité)
      int   tableauISO[] = {100, 200, 400, 800, 1600, 3200, 6400};
      byte  nombreDeValeursISO = 7;
      int   ISO = 100;                      // Sensibilité de la pellicule
      byte Selection_ISO = 0;               // le nombre qui sera incrémenté et décrémenté
      byte etat_boutonISO;                  // lecture de l'état des boutons (un seul à la fois mais une variable suffit)
      byte memoire_ISO_plus = HIGH;         // état relâché par défaut
      byte memoire_ISO_moins = HIGH;

// GESTION A (ouverture)
      float tableauA[] = {1, 1.4, 2, 2.8, 4, 5.6, 8, 11, 15, 22, 32};
      byte  nombreDeValeursA = 11;
      float A = 11;                         // A est l'ouverture du diaphragme CALCULE PAR FORMULE: //  A = sqrt ( T * (pow(2, ILs)) )
      float Aaffichage =11;                 // Aarrondie est l'ouverture du diaphragme pour l'affichage à l'ecran
      byte selection_A = 0;                 // le nombre qui sera incrémenté et décrémenté
      byte etat_boutonA;                    // lecture de l'état des boutons (un seul à la fois mais une variable suffit)
      byte memoire_A_plus = HIGH;           // état relâché par défaut
      byte memoire_A_moins = HIGH;
      
// GESTION S (vitesse)
      float tableauS[] =          { 1/8000., 1/4000., 1/2000., 1/1000., 1/500., 1/250., 1/125., 1/60., 1/30., 1/15., 1/8., 1/4., 1/2., 1, 2, 4, 8};
      int   tableauAffichageS[] = { 8000,    4000,     2000,     1000,   500,    250,    125,    60,    30,    15,    8,    4,    2,   1, 2, 4, 8};
      int   nombreDeValeursS = 17;
      int   CaseTableauAffichageS=0;
      float S = 0.0000;                     // S est le temps d'obturation CALCULE PAR FORMULE:  //  S = (pow(A, 2))/(pow(2, ILs))
      
      int Sstenope = 0;
      
      byte selection_S = 0;                 // le nombre qui sera incrémenté et décrémenté
      byte etat_boutonS;                    // lecture de l'état des boutons (un seul à la fois mais une variable suffit)
      byte memoire_S_plus = HIGH;           // état relâché par défaut
      byte memoire_S_moins = HIGH;

// LOGO puppet master
      static const unsigned char PROGMEM GITS_laughingman [] = {
      0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00,
      0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0x2D, 0xA4, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01, 0xE1,
      0x2F, 0xAC, 0xC3, 0x80, 0x00, 0x00, 0x00, 0x03, 0x86, 0xBD, 0x3C, 0xD1, 0xE0, 0x00, 0x00, 0x00,
      0x0E, 0x27, 0x80, 0x00, 0xF8, 0x70, 0x00, 0x00, 0x00, 0x1C, 0x72, 0x0F, 0xF8, 0x36, 0x38, 0x00,
      0x00, 0x00, 0x39, 0xB0, 0xFF, 0xFF, 0x0E, 0x8C, 0x00, 0x00, 0x00, 0x71, 0xC3, 0xFF, 0xFF, 0xE5,
      0x06, 0x00, 0x00, 0x00, 0xE6, 0x8F, 0xFF, 0xFF, 0xF8, 0x33, 0x00, 0x00, 0x01, 0xC7, 0x3F, 0xC0,
      0x03, 0xFC, 0x79, 0x80, 0x00, 0x01, 0x82, 0x7F, 0x00, 0x00, 0x7F, 0x79, 0xC0, 0x00, 0x03, 0x38,
      0xFC, 0x00, 0x00, 0x1F, 0x94, 0xC0, 0x00, 0x06, 0x49, 0xF0, 0x00, 0x00, 0x0F, 0xCE, 0x60, 0x00,
      0x06, 0xF3, 0xE0, 0x00, 0x00, 0x07, 0xEC, 0x30, 0x00, 0x0C, 0x27, 0xC0, 0x00, 0x00, 0x03, 0xE3,
      0x30, 0x00, 0x09, 0x8F, 0x80, 0x00, 0x00, 0x01, 0xF2, 0x98, 0x00, 0x19, 0xCF, 0x00, 0x00, 0x00,
      0x00, 0xFB, 0x98, 0x00, 0x19, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x79, 0x0C, 0x00, 0x30, 0x1E, 0x00,
      0x00, 0x00, 0x00, 0x3C, 0x4C, 0x00, 0x36, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x3C, 0xCC, 0x00, 0x21,
      0x3C, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x86, 0x00, 0x28, 0x78, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x06,
      0x00, 0x6F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x66, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xF8, 0x62, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xCE, 0x7F, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xC0, 0xF0,
      0x03, 0xC0, 0x03, 0xE0, 0x00, 0x00, 0x1F, 0xCE, 0xF0, 0x04, 0x60, 0x06, 0x30, 0x00, 0x00, 0x1F,
      0xC4, 0xF0, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xCE, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x1F, 0xC0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xCE, 0xF1, 0xFF, 0xFF, 0xFF,
      0xFF, 0x87, 0xFF, 0xFC, 0x4E, 0x71, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFC, 0x60, 0x71, 0xFF,
      0xFF, 0xFF, 0xFF, 0x8F, 0xFF, 0xF8, 0x66, 0x79, 0xFF, 0xFF, 0xFF, 0xFF, 0x8F, 0xFF, 0xE0, 0x23,
      0x78, 0xF0, 0x00, 0x00, 0x0F, 0x9E, 0x06, 0x00, 0x27, 0xB8, 0xF8, 0x00, 0x00, 0x0F, 0x1E, 0x06,
      0x00, 0x36, 0x3C, 0x7C, 0x00, 0x00, 0x1E, 0x3C, 0x0C, 0x00, 0x30, 0x1E, 0x3C, 0x00, 0x00, 0x3E,
      0x3D, 0x8C, 0x00, 0x19, 0xDE, 0x3F, 0x00, 0x00, 0x7C, 0x78, 0x6C, 0x00, 0x1B, 0xCF, 0x1F, 0x80,
      0x01, 0xF8, 0xFA, 0x18, 0x00, 0x0D, 0x0F, 0x8F, 0xF0, 0x07, 0xF1, 0xF1, 0x98, 0x00, 0x0C, 0x27,
      0xC3, 0xFF, 0xFF, 0xE1, 0xE6, 0x30, 0x00, 0x06, 0xC3, 0xE1, 0xFF, 0xFF, 0x87, 0xE7, 0x30, 0x00,
      0x06, 0x09, 0xF0, 0x7F, 0xFE, 0x0F, 0xDA, 0x60, 0x00, 0x03, 0x0C, 0xFC, 0x0F, 0xF0, 0x1F, 0x9C,
      0xC0, 0x00, 0x01, 0x9A, 0x7F, 0x00, 0x00, 0x7E, 0x6C, 0xC0, 0x00, 0x01, 0xC7, 0x3F, 0xC0, 0x03,
      0xFD, 0x71, 0x80, 0x00, 0x00, 0xC6, 0x8F, 0xFF, 0xFF, 0xF9, 0xA3, 0x00, 0x00, 0x00, 0x61, 0xC3,
      0xFF, 0xFF, 0xE5, 0xC6, 0x00, 0x00, 0x00, 0x39, 0xC8, 0xFF, 0xFF, 0x0E, 0x8C, 0x00, 0x00, 0x00,
      0x1C, 0x1C, 0x0F, 0xF0, 0x17, 0x18, 0x00, 0x00, 0x00, 0x0E, 0x1B, 0x00, 0x01, 0xC8, 0x70, 0x00,
      0x00, 0x00, 0x03, 0x83, 0x34, 0xFD, 0xC8, 0xE0, 0x00, 0x00, 0x00, 0x01, 0xC6, 0x74, 0xEF, 0x83,
      0x80, 0x00, 0x00, 0x00, 0x00, 0x78, 0x64, 0xB6, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x04,
      0x80, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x7F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00
      };
      
//############################################################################################################################################################################

void setup(){ 
    // Initialisation Serial
      Serial.begin(9600);
    // Initialisation oled
      display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    //display.display();                                          // logo adafruit
      delay(1000);
    // Initialisation LuxMeter
      LightSensor.begin();
      LightSensor.SetAddress(0x23);                                 // Address 0x10
      LightSensor.SetMode(Continuous_H_resolution_Mode);
    // Broches en entrées numériques
      pinMode(BPmode, INPUT);
      pinMode(BP_plus, INPUT);
      pinMode(BP_moins, INPUT);
    // Message d'acceuil sur l'ecran
      display.clearDisplay();
      display.display();
      display.drawBitmap(0, 0,  GITS_laughingman, 72, 64, 1);       // Affichage de mon dessin en position COLONNE 0, LIGNE 0     Taille du dessin 72 colones x 64 lignes de pixels
      display.display();
      
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(72,0);                                      // colonne 72 ligne 0
      display.println(F("Posemetre"));
      display.setCursor(75,45);                                     // colonne 72 ligne 45
      display.println(VersionProgramme);
      display.setCursor(75,54);                                     // colonne 72 ligne 54
      display.println(DateMiseAJours);
      display.display();
            
      delay(5000);
      
      display.clearDisplay();
      display.display();
}

//############################################################################################################################################################################     
 
void loop(){
  //***************************************************************************************************************  
  // Selection du mode
      EtatBPmode = digitalRead(BPmode);
      if (EtatBPmode != lastEtatBPmode) { 
        if (EtatBPmode == HIGH){
          MODE++;
          if (MODE==NombreDeMode){
            MODE=0;
          }
        }
      lastEtatBPmode = EtatBPmode;
      }
  //***************************************************************************************************************
  // Mode ISO  
       if (MODE==0){
            etat_boutonISO = digitalRead(BP_plus);
                if((etat_boutonISO != memoire_ISO_plus) && (etat_boutonISO == LOW) && (Selection_ISO < nombreDeValeursISO-1)){
                Selection_ISO++;
                }
            memoire_ISO_plus = etat_boutonISO;
            
            etat_boutonISO = digitalRead(BP_moins);
                if((etat_boutonISO != memoire_ISO_moins) && (etat_boutonISO == LOW) && (Selection_ISO > 0)){
                Selection_ISO--;
                }
            memoire_ISO_moins = etat_boutonISO;
      
            ISO = tableauISO[Selection_ISO];
      
            programmelightmeter();
            programmeprioriteouverture();
            programmeaffichage();
      }
    //***************************************************************************************************************
    // Mode Ouverture
       if (MODE==1){
            etat_boutonA = digitalRead(BP_plus);
                if((etat_boutonA != memoire_A_plus) && (etat_boutonA == LOW) && (selection_A < nombreDeValeursA-1)){
                selection_A++;
                }
            memoire_A_plus = etat_boutonA;
            
            etat_boutonA = digitalRead(BP_moins);
                if((etat_boutonA != memoire_A_moins) && (etat_boutonA == LOW) && (selection_A > 0)){
                selection_A--;
                }
            memoire_A_moins = etat_boutonA;
      
            A = tableauA[selection_A];
            Aaffichage = tableauA[selection_A];
      
            programmelightmeter();
            programmeprioriteouverture();
            programmeaffichage();
      }
  //***************************************************************************************************************
  // Mode Vitesse
      if (MODE==2){
            etat_boutonS = digitalRead(BP_plus);
                if((etat_boutonS != memoire_S_plus) && (etat_boutonS == LOW) && (selection_S < nombreDeValeursS-1)){
                selection_S++;
                }
            memoire_S_plus = etat_boutonS;
            
            etat_boutonS = digitalRead(BP_moins);
                if((etat_boutonS != memoire_S_moins) && (etat_boutonS == LOW) && (selection_S > 0)){
                selection_S--;
                }
            memoire_S_moins = etat_boutonS;
      
            S = tableauS[selection_S];
            CaseTableauAffichageS = selection_S;
      
            programmelightmeter();
            programmeprioritevitesse();
            programmeaffichage();
      }
  //***************************************************************************************************************
  // Mode Stenopé
      if (MODE==3){

         A=22;
        //ISO=400;
        
        programmelightmeter();
        Sstenope = ((pow(A, 2))/(pow(2, ILs)))*36,5;


      // Si S est inferieur à 0, alors on affiche le resultat en 1/s
        if(Sstenope < 0){
          secondes = 0;
          minutes = 0;
          secondes = Sstenope;
programmeaffichageStenope();
        }
  /*
      // Si S est superieur à 0 et inferieur à 60, alors on affiche le resultat en secondes
        if(Sstenope >= 0 && Sstenope < 60){
          heures = 0;
          minutes = 0;
          secondes = Sstenope;
programmeaffichageStenope();
        }
  
      // Si S est superieur à 60 et inferieur à 3600, alors on affiche le resultat en minutes
        if(Sstenope >= 60 && Sstenope < 3600){
          heures = 0;
          minutes = Sstenope/60;
          secondes = Sstenope % 60;
          programmeaffichageStenope();
        }
*/        
      // Si S est superieur à 3600 et inferieur à 216 000, alors on affiche le resultat en heures
        if(Sstenope > 0){
          heures = Sstenope/3600;
          minutes = (Sstenope%3600)/60;
          secondes = ((Sstenope%3600)%60);
          programmeaffichageStenope();
        }  

      }
}

//############################################################################################################################################################################
void programmelightmeter(){                                // lecture LuxMetre
      uint16_t lux = LightSensor.GetLightIntensity();
       // LUX = lux;
       LUX = (5*lux)+(48);   // courbe d'etalonage sous la forme Ax+B  A=913  B= -36280  X est la variation en Lux

    // Si LUX est inferieur à la plus petite valeur (la première), alors on retourne cette plus petite valeur
      if(LUX < tableauEquivalenceLuxIL[0]){
        IL = tableauIL[0];
        ILs = IL + (log(ISO/100))/(log(2)); 
        return;
      }

    // Si LUX est superieur à la plus grande valeur (la dernière), alors on retourne cette plus grande valeur
      if(LUX > tableauEquivalenceLuxIL[nombreDeValeursIL-1]){
        IL = tableauIL[nombreDeValeursIL-1];
        ILs = IL + (log(ISO/100))/(log(2)); 
        return;
      }

    // Sinon, on cherche les deux valeurs qui encadrent A
      int i = 1;
      while(LUX > tableauEquivalenceLuxIL[i]){
        i++;
      }

    // On a trouvé  les deux valeurs qui encadrent A : 
    // A est compris entre tableauEquivalenceLuxIL[i-1] et tableauEquivalenceLuxIL[i]
    // Attention, le tableau est rangé par ordre croissant : tableauEquivalenceLuxIL[i-1] < tableauEquivalenceLuxIL[i] 

      if(LUX-tableauEquivalenceLuxIL[i-1] < tableauEquivalenceLuxIL[i]-LUX){
        IL = tableauIL[i-1];
        ILs = IL + (log(ISO/100))/(log(2)); 
        return;
      }
      else{
        IL = tableauIL[i];
        ILs = IL + (log(ISO/100))/(log(2)); 
        return;
      }
}

void programmeprioritevitesse(){
    // calcul de temps de pose en fonction de l'ouverture et ISO
      A = sqrt ( S * (pow(2, ILs)) );   
      
    // Si A est inferieur à la plus petite valeur (la première), alors on retourne cette plus petite valeur
      if(A < tableauA[0]){
        Aaffichage = tableauA[0];
        return;
      }

    // Si A est superieur à la plus grande valeur (la dernière), alors on retourne cette plus grande valeur
      if(A > tableauA[nombreDeValeursA-1]){
        Aaffichage = tableauA[nombreDeValeursA-1];
        return;
      }

    // Sinon, on cherche les deux valeurs qui encadrent A
      int i = 1;
      while(A > tableauA[i]){
        i++;
      }

    // On a trouvé  les deux valeurs qui encadrent A : 
    // A est compris entre tableauS[i-1] et tableauS[i]
    // Attention, le tableau est rangé par ordre croissant : tableauA[i-1] < tableauA[i] 

      if(A-tableauA[i-1] < tableauA[i]-A){
        Aaffichage = tableauA[i-1];
        return;
      }
      else{
        Aaffichage = tableauA[i];
        return;
      } 
}

void programmeprioriteouverture(){
      S = (pow(A, 2))/(pow(2, ILs));

    // Si S est inferieur à la plus petite valeur (la première), alors on retourne cette plus petite valeur
      if(S < tableauS[0]){
        CaseTableauAffichageS = 0;
        return;
      }

    // Si T est superieur à la plus grande valeur (la dernière), alors on retourne cette plus grande valeur
      if(S > tableauS[nombreDeValeursS-1]){
        CaseTableauAffichageS = nombreDeValeursS-1;
        return;
      }

    // Sinon, on cherche les deux valeurs qui encadrent T
      int i = 1;
      while(S > tableauS[i]){
        i++;
      }

    // On a trouvé  les deux valeurs qui encadrent T : 
    // T est compris entre tableauS[i-1] et tableauS[i]
    // Attention, le tableau est rangé par ordre croissant : tableauS[i-1] < tableauS[i] 

      if(S-tableauS[i-1] < tableauS[i]-S){
        CaseTableauAffichageS = i-1;
        return;
      }
      else{
        CaseTableauAffichageS = i;
        return;
      }      
}   

void programmeaffichage(){
  
          display.clearDisplay();
          display.setTextColor(BLACK, WHITE);

          display.setCursor(0,0);                               //Colonne 0 Ligne 0 
          display.println(F("MODE"));
          display.setCursor(70,0);                              //Colonne 70 Ligne 0 
          display.println(tableauModeAffichage[MODE]);

          display.setTextColor(WHITE);
          display.setCursor(0,9);                               //Colonne 0 Ligne 9 
          display.println(F("ISO"));
          display.setCursor(70,9);                              //Colonne 70 Ligne 9 
          display.println(ISO,1);
          
          display.setCursor(0,18);                              //Colonne 0 Ligne 18 
          display.println(F("OUVERTURE"));
          display.setCursor(70,18);                             //Colonne 70 Ligne 18 
          display.println(Aaffichage,1);
          
          display.setCursor(0,27);                              //Colonne 0 Ligne 27 
          display.println(F("VITESSE"));
          //display.setCursor(70,27);                             //Colonne 70 Ligne 27 

          
          if (S >= 1){
            display.setCursor(70,27);                           //Colonne 70 Ligne 27
            display.print(tableauAffichageS[CaseTableauAffichageS],1);
            display.setCursor(90,27);                           //Colonne 90 Ligne 27
            display.println("s");
          } 
          else{  
            display.setCursor(70,27);                           //Colonne 70 Ligne 27
            display.print("1/");
            display.setCursor(90,27);                           //Colonne 90 Ligne 27
            display.println(tableauAffichageS[CaseTableauAffichageS],1);
          }
          
          display.setCursor(0,36);                              //Colonne 0 Ligne 36
          display.println(F("IL"));
          display.setCursor(70,36);                             //Colonne 70 Ligne 36
          display.println(IL,1);
          
          display.setCursor(0,45);                              //Colonne 0 Ligne 45
          display.println(F("ILs"));
          display.setCursor(70,45);                             //Colonne 70 Ligne 45
          display.println(ILs,1);   

          display.setCursor(0,54);                              //Colonne 0 Ligne 54
          display.println(F("Lux"));
          display.setCursor(70,54);                             //Colonne 70 Ligne 54
          display.println(LUX,1);  
                 
          display.display();
          delay (DelayRafraichissement);
}

void programmeaffichageStenope(){
  
          display.clearDisplay();
          display.setTextColor(BLACK, WHITE);

          display.setCursor(0,0);                               //Colonne 0 Ligne 0 
          display.println(F("MODE"));
          display.setCursor(30,0);                              //Colonne 70 Ligne 0 
          display.println(tableauModeAffichage[MODE]);

          display.setTextColor(WHITE);
          display.setCursor(0,9);                               //Colonne 0 Ligne 9 
          display.println(F("ISO"));
          display.setCursor(24,9);                              //Colonne 24 Ligne 9 
          display.println(ISO,1);
          
          display.setCursor(60,9);                              //Colonne 60 Ligne 9 
          display.println(F("A"));
          display.setCursor(72,9);                              //Colonne 72 Ligne 9 
          display.println(A,1);
          
          display.setCursor(0,18);                              //Colonne 0 Ligne 18
          display.println(F("IL"));
          display.setCursor(18,18);                             //Colonne 18 Ligne 18
          display.println(IL,1);
          
          display.setCursor(32,18);                              //Colonne 42 Ligne 18
          display.println(F("ILs"));
          display.setCursor(56,18);                             //Colonne 66 Ligne 18
          display.println(ILs,1);   

          display.setCursor(70,18);                              //Colonne 90 Ligne 18
          display.println(F("Lux"));
          display.setCursor(94,18);                             //Colonne 114 Ligne 18
          display.println(LUX,1);  

          display.setTextColor(BLACK, WHITE);
          display.setCursor(0,27);                              //Colonne 0 Ligne 27 
          display.println(F("VITESSE (sec)"));
          display.setCursor(90,27);                              //Colonne 90 Ligne 27
          display.println(Sstenope,1);
          
          display.setTextColor(WHITE);
          display.println(F("SECONDES"));
          display.setCursor(0,45);                              //Colonne 0 Ligne 45
          display.println(F("MINUTES"));
          display.setCursor(0,54);                              //Colonne 0 Ligne 54
          display.println(F("HEURES"));
              
          if (S < 0){      
            display.setCursor(70,36);                           //Colonne 90 Ligne 27
            display.println(secondes,1);
            display.setCursor(90,36);                           //Colonne 70 Ligne 27
            display.print("1/");
            
            display.setCursor(70,45);                           //Colonne 70 Ligne 27
            display.print(minutes,1);
            display.setCursor(90,45);                           //Colonne 90 Ligne 27
            display.println("Min");
         
            display.setCursor(70,54);                           //Colonne 70 Ligne 27
            display.print(heures,1);
            display.setCursor(90,54);                           //Colonne 90 Ligne 27
            display.println("h");
          }
          
          if (S >= 0){
            display.setCursor(70,36);                           //Colonne 70 Ligne 27
            display.print(secondes,1);
            display.setCursor(90,36);                           //Colonne 90 Ligne 27
            display.println("s");
         
            display.setCursor(70,45);                           //Colonne 70 Ligne 27
            display.print(minutes,1);
            display.setCursor(90,45);                           //Colonne 90 Ligne 27
            display.println("Min");
         
            display.setCursor(70,54);                           //Colonne 70 Ligne 27
            display.print(heures,1);
            display.setCursor(90,54);                           //Colonne 90 Ligne 27
            display.println("h");
          }       
          display.display();  
}

