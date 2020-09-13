#include <fuct1.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioct1.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <header.h>

PIO *pioa = NULL;
void FIQ_handler(void);
AIC *aic = NULL;
TC *tc = NULL;
unsigned int counter = 100;

int main(int argc, const char *argv[]) {
  unsigned int tmp;
  unsigned int bitmask;
  unsigned int next;

  STARTUP;

  pioa->OER = 0x10; // 00010000
  pioa->PER = 0x10; // 00010000

  tmp = pioa->ISR;

  while (1) {

    switch (tmp) {
    case 'e':
      CLEANUP;
      return 0;
    case 'u':
      counter += 20;
      break;
    case 'v':
      counter -= 20;
      break;
    }

    if (counter > 100) {
      counter = 100;
    }

    if (counter < 0) {
      counter = 0;
    }

    generate_pwm(10, counter);
  }

  CLEANUP;

  return 0;
}

void generate_pwm(int cycles, int duty_cycle) {
  int j;
  for (j = 0; j < cycles; j++) {
    int i;
    for (i = 0; i < duty_cycle; i++) {
      pioa->CODR = 0x10;
    }
    for (i = duty_cycle; i < 100; i++) {
      pioa->SORD = 0x10;
    }
  }
}

void FIQ_handler(void) {}