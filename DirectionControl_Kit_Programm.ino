#include <Joystick.h>
Joystick_ Joystick(0x23, JOYSTICK_TYPE_GAMEPAD, 12, 0, false, false, false, false, false, false, false, false, false, false, false);

#include <EEPROM.h>

// Anschlusspins für die Funktionen
int directionForward = 2;
int directionBackwards = 4;
int indicatorLeft = 3;
int indicatorRight = 5;
int ignition = 7;

// Hilfsvariablen für Tastenstatus
bool ignitionPressed = false;
bool indicatorLeftPressed = false;
bool indicatorRightPressed = false;

// Hilfsvariablen zum Umschalten des Modus
int mode = 0;
unsigned long startTime = 0;

// Neue Variable für die Modusanzeigetaste
bool isModeButtonHeld = true;
unsigned long backwardsPressStartTime = 0;

void setup() {
  Joystick.begin();

  pinMode(directionForward, INPUT_PULLUP);
  pinMode(directionBackwards, INPUT_PULLUP);
  pinMode(indicatorLeft, INPUT_PULLUP);
  pinMode(indicatorRight, INPUT_PULLUP);
  pinMode(ignition, INPUT_PULLUP);

  // Lesen von gespeicherten Werten
  mode = EEPROM.read(0); // Liest den Modus an Adresse 0
  if (mode > 3) {
    mode = 0; // Sicherstellen, dass der Wert im gültigen Bereich liegt
  } 

  isModeButtonHeld = EEPROM.read(1); // Liest den Status von isModeButtonHeld an Adresse 1
  if (isModeButtonHeld != 0 && isModeButtonHeld != 1) {
    isModeButtonHeld = true; // Standardwert, wenn ungültiger Wert gefunden wird
  }
}

void loop() {
  // Wechsel des Modus
  if (digitalRead(2) == LOW && digitalRead(4) == LOW && digitalRead(7) == LOW) {
    if (startTime == 0) {
      startTime = millis();
    } else if (millis() - startTime >= 5000) {
      mode = (mode + 1) % 4; // Zählt von 0 bis 3 und springt dann wieder auf 0
      EEPROM.write(0, mode); // Speichert den neuen Wert an Adresse 0
      startTime = 0;
    }
  } else {
    startTime = 0;
  }

  // Umschalten der Modusanzeigetaste
  if (digitalRead(indicatorLeft) == LOW && digitalRead(ignition) == LOW) {
    if (digitalRead(directionBackwards) == LOW) {
      if (backwardsPressStartTime == 0) {
        backwardsPressStartTime = millis();
      } else if (millis() - backwardsPressStartTime >= 5000) {
        isModeButtonHeld = !isModeButtonHeld; // Umschalten
        EEPROM.write(1, isModeButtonHeld); // Speichern des neuen Status
        backwardsPressStartTime = 0; // Zurücksetzen
      }
    } else {
      backwardsPressStartTime = 0; // Zurücksetzen, wenn directionBackwards losgelassen wird
    }
  }

  // Moduslogik
  switch (mode) {
    case 0:
      if (isModeButtonHeld) {
        Joystick.pressButton(8);
      } else {
        Joystick.releaseButton(8);
      }
      Joystick.releaseButton(9);
      Joystick.releaseButton(10);
      Joystick.releaseButton(11);

      handleInputs();
      break;

    case 1:
      if (isModeButtonHeld) {
        Joystick.pressButton(9);
      } else {
        Joystick.releaseButton(9);
      }
      Joystick.releaseButton(8);
      Joystick.releaseButton(10);
      Joystick.releaseButton(11);

      handleInputs();
      break;

    case 2:
      if (isModeButtonHeld) {
        Joystick.pressButton(10);
      } else {
        Joystick.releaseButton(10);
      }
      Joystick.releaseButton(8);
      Joystick.releaseButton(9);
      Joystick.releaseButton(11);

      handleInputs();
      break;

    case 3:
      if (isModeButtonHeld) {
        Joystick.pressButton(11);
      } else {
        Joystick.releaseButton(11);
      }
      Joystick.releaseButton(8);
      Joystick.releaseButton(9);
      Joystick.releaseButton(10);

      handleInputs();
      break;
  }

  Joystick.setButton(1, !digitalRead(directionForward));
  Joystick.setButton(2, !digitalRead(directionBackwards));
}

// Hilfsfunktion zur Verarbeitung der Inputs
void handleInputs() {
  if (digitalRead(ignition) == LOW && !ignitionPressed) {
    ignitionPressed = true;
    shortPress(0, 300);
  }

  if (digitalRead(ignition) == HIGH && ignitionPressed) {
    ignitionPressed = false;
    shortPress(0, 300);
  }

  if (digitalRead(indicatorLeft) == LOW && !indicatorLeftPressed) {
    indicatorLeftPressed = true;
    shortPress(3, 300);
  }

  if (digitalRead(indicatorLeft) == HIGH && indicatorLeftPressed) {
    indicatorLeftPressed = false;
    shortPress(5, 300);
  }

  if (digitalRead(indicatorRight) == LOW && !indicatorRightPressed) {
    indicatorRightPressed = true;
    shortPress(4, 300);
  }

  if (digitalRead(indicatorRight) == HIGH && indicatorRightPressed) {
    indicatorRightPressed = false;
    shortPress(6, 300);
  }
}

void shortPress(int button, int duration) {
  Joystick.pressButton(button);
  delay(duration);
  Joystick.releaseButton(button);
}
