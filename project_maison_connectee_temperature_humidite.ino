#include <Adafruit_SHT31.h>
#include <Servo.h>

// *********************************** ACTIONNEURS ***********************************
class Actionneur { // classe mere
protected:
  int pin;         // broche pour le pinout
  bool etat;       // etat ON/OFF = allume/eteint

public:
  Actionneur(int p) : pin(p) {
    pinMode(pin, OUTPUT);       // determine le pinout
    digitalWrite(pin, LOW);     // tout OFF au démarrage
    etat = false;               // tout OFF au démarrage
  }

  virtual void actionner() = 0; // sera definit au cas par cas pour chaque composant
  virtual void arreter() = 0;   // sera definit au cas par cas pour chaque composant
  bool getEtat() { return etat; }
};

// *********************************** LED ***********************************
class LED : public Actionneur { // classe fille
public:
  LED(int p) : Actionneur(p) {} // declaration de la broche a utiliser pour le branchement

  void actionner() override {   // redefinition pour LED
    digitalWrite(pin, HIGH);    // allume LED
    etat = true;                // LED allumee
  }

  void arreter() override {     // redefinition pour LED
    digitalWrite(pin, LOW);     // éteint LED
    etat = false;               // LED eteinte
  }

  void clignoter(int delai = 200) {
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

/* ===================== VENTILATEUR DIGITAL ===================== */
class Ventilateur : public Actionneur { // classe fille
public:
  Ventilateur(int p) : Actionneur(p) {} // declaration de la broche a utiliser pour le branchement

  void actionner() override {           // redefinition pour Ventilateur
    digitalWrite(pin, LOW);             // Ventilateur allume
    etat = true;                        // Ventilateur allume
  }

  void arreter() override {             // redefinition pour Ventilateur
    digitalWrite(pin, HIGH);            // Ventilateur eteint
    etat = false;                       // Ventilateur eteint
  }
};

/* ===================== FENETRE ===================== */
class Fenetre {                                            // classe à part
  Servo servo;
  int pin;
  bool ouverte;                                            // equivalent a etat pour les classes precedentes

public:
  Fenetre(int p) : pin(p), ouverte(false) {                // declaration de la broche a utiliser pour le branchement et definition de l'etat par defaut
    servo.attach(pin); 
    servo.write(0);    
  }

  void ouvrir()  { servo.write(180); ouverte = true; } 
  void fermer()  { servo.write(0);   ouverte = false; } 
  bool getEtat() { return ouverte; }
};

/* ===================== CAPTEUR T/H ===================== */
class CapteurTH { // plus de ": public Actionneur"
  Adafruit_SHT31 sht31; 
  float temperature;    
  float humidite;       
  float seuilT;         
  float seuilH;         

public:
  CapteurTH(float t, float h) : seuilT(t), seuilH(h), temperature(0), humidite(0) {}

  void begin() { sht31.begin(0x44); } 

  void mesurer() {
    temperature = sht31.readTemperature(); 
    humidite    = sht31.readHumidity();    
  }

  float getTemp() { return temperature; }
  float getHum()  { return humidite; }

  bool depasseSeuilTemp() { return temperature > seuilT; }
  bool depasseSeuilHum()  { return humidite > seuilH; }
};


/* ===================== CARTE ESP ===================== */
class CarteESP8266 {
private:
  LED led;                 
  Buzzer buzzer;           
  Ventilateur ventilateur; 
  Fenetre fenetre;       
  CapteurTH capteurTH;     

public:
  CarteESP8266()
    : led(D3),                  
      buzzer(D7),                
      ventilateur(D5),           
      fenetre(D6),              
      capteurTH(30.0, 70.0) {}   // seuil 25°C et 70% humidité

  void setup() {
    Serial.begin(9600);      
    capteurTH.begin();        
    led.arreter();           
    buzzer.arreter();        
    ventilateur.arreter();   
    fenetre.fermer();       
  }

  void loop() {
    capteurTH.mesurer();
    Serial.print("Temperature: ");
    Serial.print(capteurTH.getTemp());
    Serial.println(" °C");

    Serial.print("Humidite: ");
    Serial.print(capteurTH.getHum());
    Serial.println(" %");


    // ===================== Vérif seuil temp =====================
    if (capteurTH.depasseSeuilTemp()) {
      led.actionner();                      
      buzzer.actionner();               
      ventilateur.actionner();          
      fenetre.ouvrir();                
    }
    // ===================== Vérif seuil humidité =====================
    if (capteurTH.depasseSeuilHum()) {
      led.actionner();  
      buzzer.actionner();               
      ventilateur.actionner();          
      fenetre.ouvrir();                
    }

    // ===================== Sinon tout OFF =====================
    if (!capteurTH.depasseSeuilTemp() && !capteurTH.depasseSeuilHum()) {
      led.arreter();
      buzzer.arreter();
      ventilateur.arreter();
      fenetre.fermer();
    }

    delay(3000); // pause boucle
  }
};

CarteESP8266 carte;

void setup() { carte.setup(); }
void loop()  { carte.loop();  }
