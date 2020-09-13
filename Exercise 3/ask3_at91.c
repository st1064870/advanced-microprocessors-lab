#include <fcntl.h>
#include <header.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOW 0
#define HIGH 1

PIO *pioa = NULL;
void FIQ_handler(void);
AIC *aic = NULL;
TC *tc = NULL;

// Variables
int ledState_mon = LOW;
int ledState_dek = LOW;
int ledState_state = LOW;
int secs = 0;
int counter = 0;
int temp = 0;
int pressed = 1;
int butt_pr = 0;
int released = 0;
int interrupt = 0;

unsigned long previousMillis = 0;
extern unsigned long currentMillis = 0;
// interval at which to blink (milliseconds)
const long interval = 1000; // 1Hz
const long interval2 = 500; // 2Hz

void FIQ_handler() {
  unsigned int fiq = 0;
  fiq = pioa->IPR;

  if (fiq & (1 << PIOA_ID)) {
    butt_pr += pressed;
  }
}

void reset() {
  currentMillis = 0;
  previousMillis = 0;
  ledState_mon = LOW;
  ledState_dek = LOW;
  ledState_state = LOW;
  counter = 0;
  temp = 0;
  pressed = 1;
  butt_pr = 0;
  interrupt = 0;
}

void butt_hold() {
  static unsigned int start = 0;
  if (pressed) {
    if (millis() - start >= 1000) {
      reset();
    }
  }
}

void setup() {
  pioa->PER = 0xca;  // 1100 1010 Enable 8, 7, 4, 2 lines
  pioa->PUER = 0x2;  // 0000 0010 Enable pull-up on 2
  pioa->ODR = 0x2;   // 0000 0010 Enable input on 8
  pioa->OER = 0xc8;  // 1100 1000 Enable output on 8, 7, 4 lines
  pioa->CODR = 0xc8; // 1100 1000 Clear output on 8,7, 4 lines
  //  Serial.begin(9600);
  aic->ICCR = (1 << PIOA_ID); // Clear AIC from interrupts
  pioa->IER = 0x01;           // 0000 0001 Enable line 1 interrupts
  //   pinMode(2, INPUT_PULLUP);
  //   pinMode(4, OUTPUT);
  //   pinMode(7, OUTPUT);
  //   pinMode(8, OUTPUT);
}

void loop() {
  // butt_hold();
  unsigned long currentMillis = millis();
  if (currentMillis == 59000) {
    currentMillis = 0; // TO CHECK previous =0;
  }
  if (butt_pr % 2 != 0) {
    if (currentMillis - previousMillis >= interval2) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      interrupt = 1;
      // ENTER HOLD STATE
      // digitalWrite(4, ledState_mon);
      pioa->CORD = digitalWrite(7, ledState_dek);
      pioa->CODR = ledState_state = HIGH;
      // digitalWrite(8, HIGH);
      ledState_state = LOW;
      pioa->CODR =
      // digitalWrite(8, LOW);
      pioa->CODR =
    } else {
      interrupt = 0;
      reset();
    }
  }

  if (!interrupt) {
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      // set the LED with the ledState of the variable:
      ledState_mon = HIGH;
      // digitalWrite(4, HIGH);
      pioa->CODR = ledState_mon = LOW;
      // digitalWrite(4, LOW);
      pioa->CODR = if (currentMillis % 10000 == 0) {
        counter = counter + 1;
        temp = counter;
        // Serial.print(temp);
        printf("%d\n", temp);
      }
      if (temp > 0) {
        ledState_dek = HIGH;
        // digitalWrite(7, HIGH);
        ledState_dek = LOW;
        // digitalWrite(7, LOW);
        temp -= 1;
      }
    }
  }
}

void digitalWrite(int pin, unsigned int value) {
  switch (value) {
  case LOW:
    pioa->CODR = pioa->CODR | (1 << pin) break;
  case HIGH:
    pioa->SODR = pioa->SODR | (1 << pin) break;
  }
}

int main(int argc, const char *argv[]) {
  STARTUP;

  setup();
  while (getchar() != 'e') {
    loop();
  }

  pioaaic->IDCR = (1 << PIOA_ID); // Stop AIC interrupts
  CLEANUP;
  return 0;
}
