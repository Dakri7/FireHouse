/*
FireHouse
Copyright (C) 2024 FF-Hechtsheim

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

#define RST_NONE 0
#define RST_WAITING 1
#define RST_RECOVERING 2


// ===================== CONFIGURATION =======================================

#define WINDOW_COUNT 4

//Pin configuration
const int floaterPins[WINDOW_COUNT] = {0, 1, 2, 3};
const int lightPins[WINDOW_COUNT] = {4, 5, 6, 7};
const int valvePins[WINDOW_COUNT] = {8, 9, 10, 11};
const int finishedPin = 12;
const int pumpPin = 13;
const int normalModeReset = 14;
const int hardModeReset = 15;


//Hard mode only
const int minReigniteDelay = 1000 * 25; //ms
const int maxReigniteDelay = 1000 * 30; //ms

const int pumpDuration = 1000 * 60 * 5;
const int resetDelay = 1000 * 30;
const int resetCooldown = 1000 * 2;
// ===========================================================================

bool hardMode = false;
long windowState[WINDOW_COUNT] = {};
long pumpOffTime = 0;
long resetTime = 0;
int resetStage = RST_NONE;
bool hardReset = false;
bool allExtinguished;

void setup() {
  for(int i = 0; i < WINDOW_COUNT; i++){
    pinMode(floaterPins[i], INPUT_PULLUP);
    pinMode(lightPins[i], OUTPUT);
    pinMode(valvePins[i], OUTPUT);

    digitalWrite(lightPins[i], LOW);
    digitalWrite(valvePins[i], HIGH);

    windowState[i] = 0;
  }
  pinMode(hardModeReset, INPUT_PULLUP);
  pinMode(normalModeReset, INPUT_PULLUP);
  pinMode(finishedPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(finishedPin, HIGH);
  digitalWrite(pumpPin, LOW);
  allExtinguished = false;
}

void reset(bool hard){
  hardMode = hard;
  for(int i = 0; i < WINDOW_COUNT; i++){
    windowState[i] = 0;
  }
  allExtinguished = false;
  resetTime = millis() + 2000;
  resetStage = RST_RECOVERING;
}

void loop() {
  bool normlRst = digitalRead(normalModeReset) == LOW;
  bool hardRst = digitalRead(hardModeReset) == LOW;

  if(resetStage == RST_NONE && (normlRst || hardRst)){
    hardReset = hardRst;
    if(allExtinguished){
      resetTime = millis();
    } else {
      resetTime = millis() + resetDelay;

      for(int i = 0; i < WINDOW_COUNT; i++){
        windowState[i] = millis() + resetDelay;
      }
    }
    resetStage = RST_WAITING;
  }

  if(resetStage == RST_WAITING && resetTime < millis()){
    reset(hardReset);
    return;
  }

  if(resetStage == RST_RECOVERING && resetTime < millis()){
    resetStage = RST_NONE;
    resetTime = 0;
  }

  if(allExtinguished){
    digitalWrite(finishedPin, LOW);

    if(pumpOffTime < millis()){
      digitalWrite(pumpPin, HIGH);
    } else {
      digitalWrite(pumpPin, LOW);
    }
  } else {
    digitalWrite(finishedPin, HIGH);
    allExtinguished = true;   // This is set to false later if the game is not finished
    pumpOffTime = millis() + pumpDuration;  // Pump runs for a duration longer then the game is running
    digitalWrite(pumpPin, LOW);
    
    //Set current time to 0 for easy mode, so valves never close again
    long currTime = hardMode ? millis(): 0;
    
    for(int i = 0; i < WINDOW_COUNT; i++){
      //Update window states
      //Note that low is a closed switch because the gate is hooked up to a pullup circuit
      if(digitalRead(floaterPins[i]) == LOW){
        windowState[i] = currTime + random(minReigniteDelay, maxReigniteDelay);
      }

      allExtinguished = allExtinguished && windowState[i] > currTime;

      //Set pins according to window state
      if(windowState[i] <= currTime){         //BURNING
        digitalWrite(lightPins[i], LOW);
        digitalWrite(valvePins[i], HIGH);
      } else if(windowState[i] > currTime){   //EXTINGUISHED
        digitalWrite(lightPins[i], HIGH);
        digitalWrite(valvePins[i], LOW);
      }
    }
  }
}
