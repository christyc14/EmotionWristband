
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ----------------------
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 13
2) Fades an LED to User's Live HeartBeat
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
 ----------------------       ----------------------  ----------------------
*/
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int EmoLite = 12;


// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile int PBI = 600;             // PBI stands for Previous Beat Interval, assigned at the same at as rate[8]
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse


void setup() {

  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);
  matrix.begin(0x70);  // pass in the address
  pinMode(EmoLite, OUTPUT);
  Serial.begin(57600);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS

}

static const uint8_t PROGMEM
smile_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
},
neutral_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100
},
frown_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

//  Where the Magic Happens
void loop() {
  matrix.clear();
  serialOutput() ;

  if (QS == true) {    // A Heartbeat Was Found
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat

    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
//insert something so when you don't find a pulse, it loops back to the beginning......... MAKE IT WORK
    if (IBI - PBI <= -90) {             //when the difference between the most recent IBI and the one before IBI is less than -90
      delay(1000);
      if (IBI - PBI <= -90) {
        delay(1000);
        if (IBI - PBI <= -90) {
          digitalWrite(EmoLite, HIGH);
          matrix.setBrightness(3);
          matrix.setTextSize(1);
          matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
          matrix.setTextColor(LED_ON);
          for (int8_t x = 0; x >= -45; x--) {
            matrix.clear();
            matrix.setCursor(x, 0);
            matrix.print("In 2 3 4");
            matrix.writeDisplay();
            delay(100);
          }
          for (int8_t x = 9; x >= -90; x--) {
            matrix.clear();
            matrix.setCursor(x, 0);
            matrix.print("Hold 2 3 4 5 6 7");
            matrix.writeDisplay();
            delay(100);
          }
          for (int8_t x = 16; x >= -103; x--) {
            matrix.clear();
            matrix.setCursor(x, 0);
            matrix.print("Out 2 3 4 5 6 7 8");
            matrix.writeDisplay();
            delay(100);
          }
          matrix.setRotation(0);
          delay(100000);
        }
        else {
          digitalWrite(EmoLite, LOW);
          matrix.clear();
        }
      }
      else {
        digitalWrite(EmoLite, LOW);
        matrix.clear();
      }
    }
    else {
      digitalWrite(EmoLite, LOW);
      matrix.clear();

    }
    // QS FLAG IS NOT CLEARED INSIDE THIS ISR
  }
}












