/*
FireHouse
Copyright (C) 2024  FF-Hechtsheim

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// ===================== CONFIGURATION =======================================
#define WINDOW_COUNT 4

const int floaterPins[WINDOW_COUNT] = {};
const int lightPins[WINDOW_COUNT] = {};
const int valvePins[WINDOW_COUNT] = {};
const int finishedPin = NULL;
const int hardModePin = NULL;
const int pumpPin = NULL;

const int minReigniteDelay = 15000;
const int maxReigniteDelay = 20000;
const int pumpDuration = 1000 * 60 * 5;
// ===========================================================================

long windowState[WINDOW_COUNT] = {};
long pumpOffTime = 0;
bool allExtinguished;

void setup() {
  for(int i = 0; i < WINDOW_COUNT; i++){
    pinMode(floaterPins[i], INPUT_PULLUP);
    pinMode(lightPins[i], OUTPUT);
    pinMode(valvePins[i], OUTPUT);

    digitalWrite(lightPins[i], HIGH);
    digitalWrite(valvePins[i], LOW);

    windowState[i] = 0;
  }
  pinMode(hardModePin, INPUT_PULLUP);
  pinMode(finishedPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(finishedPin, LOW);
  digitalWrite(pumpPin, HIGH);
  allExtinguished = false;
}

void loop() {
  if(allExtinguished){
    digitalWrite(finishedPin, HIGH);

    if(pumpOffTime < millis()){
      digitalWrite(pumpPin, LOW);
    } else {
      digitalWrite(pumpPin, HIGH);
    }
  } else {
    allExtinguished = true;   // This is set to fgalse later if the game is not finished
    pumpOffTime = millis() + pumpDuration;  // Pump runs for a duration longer then the game is running
    digitalWrite(pumpPin, HIGH);
    
    //Set current time to 0 for easy mode, so valves never close again
    long currTime = digitalRead(hardModePin) == LOW ? millis(): 0;
    
    for(int i = 0; i < WINDOW_COUNT; i++){
      //Update window states
      //Note that low is a closed switch because the gate is hooked up to a pullup circuit
      if(digitalRead(floaterPins[i]) == LOW){
        windowState[i] = currTime + random(minReigniteDelay, maxReigniteDelay);
      }

      allExtinguished = allExtinguished && windowState[i] > currTime;

      //Set pins according to window state
      if(windowState[i] <= currTime){         //BURNING
        digitalWrite(lightPins[i], HIGH);
        digitalWrite(valvePins[i], LOW);
      } else if(windowState[i] > currTime){   //EXTINGUISHED
        digitalWrite(lightPins[i], LOW);
        digitalWrite(valvePins[i], HIGH);
      }
    }
  }
}
