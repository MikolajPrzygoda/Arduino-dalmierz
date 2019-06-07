#include <TM1638lite.h>
#include <NewPing.h>

#define TM_STROBE_PIN 4
#define TM_CLK_PIN 7
#define TM_DATA_PIN 8
#define SENSOR_TRIG_PIN 11
#define SENSOR_ECHO_PIN 12

// Masks for buttons 1 and 2
#define BUTTON_S1 1
#define BUTTON_S2 2

#define MEASURE_COUNT 50
#define MAX_DISTANCE 200

enum phase {
  INIT,
  MEASURE,
  SHOW_RESULT
};

TM1638lite tm(4, 7, 8);
NewPing sonar(SENSOR_TRIG_PIN, SENSOR_ECHO_PIN, MAX_DISTANCE);

phase currentPhase = INIT;

// Phase 1 variables
unsigned short currentCycle = 0;
unsigned short maxCycles = 50; //500ms on, 500ms off
bool textShown = true;

long echoTime;
double dist;

void setup() {
  tm.reset();
  tm.displayText("--------");
}

void loop() {
  switch(currentPhase){
    case INIT:
      if(currentCycle == maxCycles){
        currentCycle = 0;
        if(textShown)
          tm.displayText("        ");
        else
          tm.displayText("--------");
        textShown = !textShown;
      }
      else
        currentCycle++;

      // Start measuring when button 1 is pushed
      if(tm.readButtons() & BUTTON_S1){
        currentPhase = MEASURE;
        tm.displayText("        ");
      }
        
      break;

    case MEASURE:
      // Measure distance
      echoTime = sonar.ping_median(MEASURE_COUNT);
      dist = echoTime / (double) 57;
      
      // Show result on display
      for(int i = 0; i < 4; i++){
        double digit = dist / pow(10, 2 - i);
        uint8_t value = (int) digit % 10;

        if(i == 2){ // Add a dot to the third segment
          uint8_t ssValue = ss['0' + value]; // Get on/off segments from library conversion table
          ssValue |= 0b10000000; // Turn on the dot segment
          tm.displaySS(i, ssValue);
        }
        else if(i != 3 && value == 0){ //
          tm.displayASCII(i, ' ');
        }
        else{
          tm.displayHex(i, value);
        }
      }
      
      // Advance to the next phase
      currentPhase = SHOW_RESULT;
      break;

    case SHOW_RESULT:
      if(tm.readButtons() & BUTTON_S2){
        tm.displayText("--------");
        currentCycle = 0;
        textShown = true;
        currentPhase = INIT;
      }
      break;
  }

  delay(10);
}
