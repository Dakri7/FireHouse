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

#define WINDOW_COUNT 4
#define BURNING 0
#define EXTINGUISHED 1

//TODO Pin layout
const int floaterPins[WINDOW_COUNT] = {};
const int lightPins[WINDOW_COUNT] = {};
const int valvePins[WINDOW_COUNT] = {};
const int finishedPin = NULL;

int windowState[WINDOW_COUNT] = {};
bool allExtinguished;

void setup() {
  for(int i = 0; i < WINDOW_COUNT; i++){
    pinMode(floaterPins[i], INPUT_PULLUP);
    pinMode(lightPins[i], OUTPUT);
    pinMode(valvePins[i], OUTPUT);

    digitalWrite(lightPins[i], HIGH);
    //TODO check whether HIGH is open or closed
    digitalWrite(valvePins[i], HIGH);

    windowState[i] = BURNING;
  }

  allExtinguished = false;
}

void loop() {
  allExtinguished = true;
  for(int i = 0; i < WINDOW_COUNT; i++){
    //Update window states
    //Note that low is a closed switch because the gate is hooked up to a pullup circuit
    if(digitalRead(floaterPins[i]) == LOW){
      windowState[i] = EXTINGUISHED;
    }

    allExtinguished = allExtinguished && windowState[i] == EXTINGUISHED;

    //Set pins according to window state
    if(windowState[i] == BURNING){
      digitalWrite(lightPins[i], HIGH);
      //TODO check whether HIGH is open or closed
      digitalWrite(valvePins[i], HIGH);
    } else if(windowState[i] == EXTINGUISHED){
      digitalWrite(lightPins[i], LOW);
      //TODO check whether HIGH is open or closed
      digitalWrite(valvePins[i], LOW);
    }
  }

  if(allExtinguished){
    digitalWrite(finishedPin, HIGH);
  }
}
