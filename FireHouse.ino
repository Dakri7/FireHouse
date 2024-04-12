#define WINDOW_COUNT 4
#define BURNING 0
#define EMPTYING 1
#define EXTINGUISHED 2

//TODO Pin layout
const int floaterPins[WINDOW_COUNT] = {};
const int lightPins[WINDOW_COUNT] = {};
const int valvePins[WINDOW_COUNT] = {};

int windowState[WINDOW_COUNT] = {};
bool allExtinguished;

const int valveDuration = 10000;

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
  for(int i = 0; i < WINDOW_COUNT; i++){
    //Update window states
    //Note that low is a closed switch because the gate is hooked up to a pullup circuit
    if(digitalRead(floaterPins[i]) == LOW){
      windowState[i] = EMPTYING;
    }

    allExtinguished = allExtinguished && (windowState[i] == EMPTYING || windowState[i] == EXTINGUISHED);

    //Set pins according to window state
    if(windowState[i] == BURNING){
      digitalWrite(lightPins[i], HIGH);
      //TODO check whether HIGH is open or closed
      digitalWrite(valvePins[i], HIGH);
    } else if(windowState[i] == EMPTYING){
      digitalWrite(lightPins[i], LOW);
      //TODO check whether HIGH is open or closed
      digitalWrite(valvePins[i], LOW);
    } else {
      digitalWrite(lightPins[i], LOW);
      //TODO check whether HIGH is open or closed
      digitalWrite(valvePins[i], HIGH);
    }

  }

}
