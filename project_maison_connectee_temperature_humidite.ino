#include <Adafruit_SHT31.h> // capteur de temperature et d'humidite
#include <Servo.h>          // servomoteur

// *********************************** ACTIONNEURS ***********************************
class Actionneur {                // classe mere
protected:
  int pin;                        // broche pour le pinout
  bool etat;                      // etat ON/OFF = allume/eteint

public:
  Actionneur(int p) : pin(p) {
    pinMode(pin, OUTPUT);         // determine le pinout
    digitalWrite(pin, LOW);       // tout OFF au démarrage
    etat = false;                 // tout OFF au démarrage
  }

  virtual void actionner() = 0;   // sera definit au cas par cas pour chaque composant
  virtual void arreter() = 0;     // sera definit au cas par cas pour chaque composant
  bool getEtat() { return etat; }
};

// *********************************** LED ***********************************
class LED : public Actionneur {     // classe fille
public:
  LED(int p) : Actionneur(p) {}     // declaration de la broche a utiliser pour le branchement

  void actionner() override {       // redefinition pour LED
    digitalWrite(pin, HIGH);        // allume LED
    etat = true;                    // LED allumee
  }

  void arreter() override {         // redefinition pour LED
    digitalWrite(pin, LOW);         // éteint LED
    etat = false;                   // LED eteinte
  }

  void clignoter(int delai = 200) { // clignotement de la LED
    actionner();            
    delay(delai);           
    arreter();              
    delay(delai);           
  }
};

// *********************************** BUZZER ***********************************
class Buzzer : public Actionneur {  // classe fille
private:

  void jouerSon() {
    for (int i = 0; i < 3; i++) {   // 3 bips
      tone(pin, 800, 200);
      delay(250);
      noTone(pin);
      delay(200);
    }
  }

public:

  Buzzer(int p) : Actionneur(p) {}  // declaration de la broche a utiliser pour le branchement

  void actionner() override {       // redefinition pour Buzzer
    jouerSon();
    etat = true;                    // Buzzer allume
  }

  void arreter() override {         // redefinition pour Buzzer
    noTone(pin);
    etat = false;                   // Buzzer eteint
  }
};

// *********************************** VENTILATEUR ***********************************
class Ventilateur : public Actionneur { // classe fille
public:
  Ventilateur(int p) : Actionneur(p) {} // declaration de la broche a utiliser pour le branchement

  void actionner() override {           // redefinition pour Ventilateur
    digitalWrite(pin, LOW);             // Ventilateur allume, notre montage nous contraint de le mettre a LOW pour l'allumer
    etat = true;                        // Ventilateur allume
  }

  void arreter() override {             // redefinition pour Ventilateur
    digitalWrite(pin, HIGH);            // Ventilateur eteint, notre montage nous contraint de le mettre a HIGH pour l'eteindre
    etat = false;                       // Ventilateur eteint
  }
};

// *********************************** FENETRE ***********************************
class Fenetre {                                            // classe à part
  Servo servo;
  int pin;
  bool ouverte;                                            // equivalent a etat pour les classes precedentes

public:
  Fenetre(int p) : pin(p), ouverte(false) {                // declaration de la broche a utiliser pour le branchement et definition de l'etat par defaut
    servo.attach(pin); 
    servo.write(0);    
  }

  void ouvrir()  { servo.write(180); ouverte = true; }     // ouverture de la fenetre
  void fermer()  { servo.write(0);   ouverte = false; }    // fermeture de la fenetre
  bool getEtat() { return ouverte; }
};

// *********************************** CAPTEUR DE TEMPERATURE ET D'HUMIDITE ***********************************
class CapteurTH {                                                                    // classe a part
  Adafruit_SHT31 sht31;
  float temperature;                                                                 // temperature mesuree par le capteur
  float humidite;                                                                    // humidite mesuree par le capteur
  float seuilT;                                                                      // seuil de temperature a comparer pour declancher ou non les actions
  float seuilH;                                                                      // seuil d'humidite a comparer pour declancher ou non les actions

public:
  CapteurTH(float t, float h) : seuilT(t), seuilH(h), temperature(0), humidite(0) {} // declaration des valeurs des seuils a comparer et initialisation des valeurs de temperature et d'humidite

  void begin() { sht31.begin(0x44); }                                                // declaration de la broche a utiliser pour le branchement, correspond a D1 ??????????????????????????????????????????????????????????????

  void mesurer() {
    temperature = sht31.readTemperature();                                           // mesure de la temperature
    humidite    = sht31.readHumidity();                                              // mesure de l'humidite
  }

  float getTemp() { return temperature; }                                            // affichage de la temperature mesuree
  float getHum()  { return humidite; }                                               // affichage de l'humidite mesuree

  bool depasseSeuilTemp() { return temperature > seuilT; }                           // comparaison de la temperature a son seuil
  bool depasseSeuilHum()  { return humidite > seuilH; }                              // comparaison de l'humidite a son seuil
};


// *********************************** CARTE ESP8266 ***********************************
class CarteESP8266 {                                                     // classe principale, a part
private:
  LED led;                                                               // instanciation de la LED
  Buzzer buzzer;                                                         // instanciation du buzzer
  Ventilateur ventilateur;                                               // instanciation du ventilateur
  Fenetre fenetre;                                                       // instanciation du servomoteur de la fenetre
  CapteurTH capteurTH;                                                   // instanciation du capteur de temperature

public:
  CarteESP8266()
    : led(D3),                                                           // declaration du pinout de la LED
      buzzer(D7),                                                        // declaration du pinout du buzzer
      ventilateur(D5),                                                   // declaration du pinout du ventilateur
      fenetre(D6),                                                       // declaration du pinout du servomoteur
      capteurTH(30.0, 70.0) {}                                           // declaration des seuils de temperature (30°C) et d'humidite (70%)

  void setup() {
    Serial.begin(9600);
    capteurTH.begin();                                                   // demarrage des mesures du capteur de temperature et d'humidite
    led.arreter();                                                       // initialisation de la LED a eteinte
    buzzer.arreter();                                                    // initialisation du buzzer a eteint
    ventilateur.arreter();                                               // initialisation du ventilateur a eteint
    fenetre.fermer();                                                    // initialisation de la fenetre a fermee
  }

  void loop() {
    capteurTH.mesurer();                                                 // mesure de la temperature et de l'humidite
    Serial.print("Temperature: ");
    Serial.print(capteurTH.getTemp());                                   // affichage de la temperature
    Serial.println(" °C");

    Serial.print("Humidite: ");
    Serial.print(capteurTH.getHum());                                    // affichage de l'humidite
    Serial.println(" %");


    // Depassement du seuil de temperature
    if (capteurTH.depasseSeuilTemp()) {                                  // actions a effectuer si le seuil de temperature est franchi
      led.clignoter();                                                   // clignotement de la LED
      buzzer.actionner();                                                // allumage du buzzer
      ventilateur.actionner();                                           // allumage du ventilateur
      fenetre.ouvrir();                                                  // ouverture de la fenetre
    }
    // Depassement du seuil d'humidité
    if (capteurTH.depasseSeuilHum()) {                                   // actions a effectuer si le seuil d'humidite est franchi
      led.clignoter();                                                   // clignotement de la LED
      buzzer.actionner();                                                // allumage du buzzer
      ventilateur.actionner();                                           // allumage du ventilateur
      fenetre.ouvrir();                                                  // ouverture de la fenetre
    }

    // Aucun depassement de seuil, situation normale
    if (!capteurTH.depasseSeuilTemp() && !capteurTH.depasseSeuilHum()) { // actions a effectuer si la situation est ou redevient normale
      led.arreter();                                                     // exctinction de la LED
      buzzer.arreter();                                                  // exctinction du buzzer
      ventilateur.arreter();                                             // exctinction du ventilateur
      fenetre.fermer();                                                  // fermeture de la fenetre
    }

    delay(3000);                                                         // delai avant prochaine mesure du capteur de temperature et d'humidite
  }
};

CarteESP8266 carte;                                                      // instanciation de la carte ESP8266

void setup() { carte.setup(); }                                          // configuration de la carte ESP8266
void loop()  { carte.loop();  }                                          // demarrage du programme principal
