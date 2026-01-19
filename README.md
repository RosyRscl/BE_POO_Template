# BE C++ – Maison connectée : Température et Humidité

## Description du projet
Ce projet a été réalisé dans le cadre du Bureau d’Étude C++ appliqué aux objets connectés.  
Son but est de concevoir une application de maison connectée basée sur une carte ESP8266, permettant de surveiller la température et l’humidité d’une pièce et déclencher automatiquement des actions lorsque des seuils prédéfinis sont dépassés.

Le projet repose sur une conception orientée objet avec l’utilisation de plusieurs classes représentant les capteurs et les actionneurs.

---

## Fonctionnement général
Le système est composé des éléments suivants :
- une carte mère (ESP8266) et son shield grove
- un capteur de température et d’humidité (SHT31)
- une LED
- un buzzer
- un ventilateur (moteur 3V-6V avec des pales)
- une fenêtre motorisée (servomoteur)

Des seuils de température et d’humidité sont définis dans le programme.

Lorsque la température ou l’humidité dépasse les seuils définis :
- la LED s’allume
- le buzzer émet un signal sonore
- le ventilateur s’allume
- la fenêtre s’ouvre afin d’aérer la pièce

Lorsqu’aucun seuil n’est dépassé ou que les mesures redescendent en dessous des seuils :
- la LED et le buzzer sont désactivés
- le ventilateur est arrêté
- la fenêtre est fermée

---

## Prérequis

### Matériel
- Carte ESP8266 (NodeMCU)
- Shield Grove
- Breadboard
- Capteur SHT31
- LED
- Buzzer
- Ventilateur
- Servomoteur :
    - Transistor P-MOS
    - Diode de roue libre
    - Résistance (100kOhm) 
- Câbles de connexion
- Maquette faisant office de maison

### Logiciel
- Arduino IDE
- Bibliothèques Arduino :
  - Adafruit_SHT31
  - Servo

---

## Compilation du programme
1. Ouvrir Arduino IDE
2. Installer les bibliothèques nécessaires via :
   ```
   'Library Manager' → Recherche de la bibliothèque souhaitée → 'Install' 
   ```
3. Sélectionner la carte :
   ```
   'Boards Manager' → Recherche de ESP8266 NodeMCU 1.0 (ESP-12E Module) → 'Install'
   ```
4. Cliquer sur **Vérifier** afin de compiler le programme

---

## Téléversement et exécution
1. Connecter la carte ESP8266 à l’ordinateur à l’aide d’un câble USB
2. Sélectionner le port série correspondant à la carte (ex : COM7)
3. Cliquer sur **Téléverser**
4. Une fois le téléversement terminé, le programme démarre automatiquement sur la carte

---

## Interaction avec le système
Le système fonctionne de manière autonome à partir des mesures du capteur.

Pour interagir avec le programme :
- **modifier** la température ou l’humidité autour du capteur SHT31
- **observer** la réaction des actionneurs (LED, buzzer, ventilateur, fenêtre)
- **consulter** les valeurs mesurées via le Moniteur Série :
  - vitesse de communication : **9600 bauds**

Les seuils utilisés dans le programme sont :
- Température : 30 °C
- Humidité : 70 %

---

## Organisation du code
- **Actionneur** : classe abstraite définissant la base commune à tous les actionneurs
- **LED**, **Buzzer**, **Ventilateur** : classes filles héritant de la classe Actionneur
- **Fenetre** : classe chargée de la gestion de l’ouverture et de la fermeture de la fenêtre via un servomoteur
- **CapteurTH** : classe chargée de l'acquisition et du transfert des données de température et d’humidité à la classe principale
- **CarteESP8266** : classe principale assurant la coordination du système

---

## Auteurs
Projet réalisé dans le cadre du BE C++ – Objets connectés par KELLY Sarah et FIHRI Firdaws, apprenties en 4ième année d'école d'ingénieur au sein du département GEI à l'INSA Toulouse.
