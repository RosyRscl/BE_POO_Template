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
  LED(int p) : Actionneur(p) {}

  void actionner() override {
    digitalWrite(pin, HIGH);  // allume LED
    etat = true;
  }

  void arreter() override {
    digitalWrite(pin, LOW);   // éteint LED
    etat = false;
  }

  void clignoter(int delai = 200) { 
    actionner();            
    delay(delai);           
    arreter();              
    delay(delai);           
  }
};

// *********************************** BUZZER ***********************************
class Buzzer : public Actionneur {
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

  Buzzer(int p) : Actionneur(p) {}

  void actionner() override {
    jouerSon();
    etat = true;
  }

  void arreter() override {
    noTone(pin);
    etat = false;
  }
};

/* ===================== VENTILATEUR DIGITAL ===================== */
class Ventilateur : public Actionneur {
public:
  Ventilateur(int p) : Actionneur(p) {}

  void actionner() override {
    digitalWrite(pin, LOW); 
    etat = true;
  }

  void arreter() override {
    digitalWrite(pin, HIGH);  
    etat = false;
  }
};

/* ===================== FENETRE ===================== */
class Fenetre {
  Servo servo;      
  int pin;          
  bool ouverte;     

public:
  Fenetre(int p) : pin(p), ouverte(false) {
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
