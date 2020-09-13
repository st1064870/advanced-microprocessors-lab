#include <fcntl.h>
#include <header.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIOA_ID 2
#define TC0_ID 17
#define BUT_IDLE 0
#define BUT_PRESSED 1
#define BUT_RELEASED 2
#define LED_IDLE 0
#define LED_FLASHING 1

void FIQ_handler(void);
PIO *pioa = NULL;
AIC *aic = NULL;
TC *tc = NULL;
unsigned int button_state = BUT_IDLE;
unsigned int led_state = LED_IDLE;

int main(int argc, const char *argv[]) {
  unsigned int gen;
  STARTUP;                    //ΑΡΧΙΚΟΠΟΙΗΣΗ ΣΥΣΤΗΜΑΤΟΣ
  tc->Channel_0.RC = 8192;    //ΠΕΡΙΟ∆ΟΣ 1 ∆ΕΥΤΕΡΟΛΕΠΤΟ
  tc->Channel_0.CMR = 0x2084; // SLOW CLOCK , WAVEFORM , DISABLE CLK ON
                              // RCCOMPARE
  tc->Channel_0.IDR = 0xFF; //ΑΠΕΝΕΡΓΟΠΟΙΗΣΗ ΟΛΩΝ ΤΩΝ∆ΙΑΚΟΠΩΝ
  tc->Channel_0.IER = 0x10; //ΕΝΕΡΓΟΠΟΙΗΣΗ ΜΟΝΟ ΤΟΥ RCCOMPARE
  aic->FFER = (1 << PIOA_ID) | (1 << TC0_ID); //ΟΙ ∆ΙΑΚΟΠΕΣ 2,17 ΕΙΝΑΙ ΤΥΠΟΥ FIQ
  aic->IECR =
      (1 << PIOA_ID) | (1 << TC0_ID); //ΕΝΕΡΓΟΠΟΙΗΣΗ ∆ΙΑΚΟΠΩΝ : PIOA & TC0
  pioa->PUER = 0x01; //ΕΝΕΡΓΟΠΟΙΗΣΗ ΣΤΗ ΓΡΑΜΜΗ 27:PULL−UP
  pioa->ODR = 0x01; //ΓΡΑΜΜΗ 27:ΛΕΙΤΟΥΡΓΙΑ ΕΙΣΟ∆ΟΥ
  pioa->CODR = 0x02;   //ΓΡΑΜΜΗ 13: ∆ΥΝΑΜΙΚΟ ΕΞΟ∆ΟΥ HIGH
  pioa->OER = 0x02;    //ΓΡΑΜΜΗ13:ΛΕΙΤΟΥΡΓΙΑ ΕΞΟ∆ΟΥ
  gen = pioa->ISR; // PIOA:ΕΚΚΑΘΑΡΙΣΗ ΑΠΟ ΤΥΧΟΝ ∆ΙΑΚΟΠΕΣ
  pioa->PER = 0x03; //ΓΡΑΜΜΕΣ 27, 13: ΓΕΝΙΚΟΥ ΣΚΟΠΟΥ
  gen = tc->Channel_0.SR; // TC0 :   ΕΚΚΑΘΑΡΙΣΗ ΑΠΟ   ΤΥΧΟΝ ∆ΙΑΚΟΠΕΣ
  aic->ICCR =
      (1 << PIOA_ID) | (1 << TC0_ID); // AIC:ΕΚΚΑΘΑΡΙΣΗ ΑΠΟ ΤΥΧΟΝ ∆ΙΑΚΟΠΕΣ
  pioa->IER = 0x01; //ΕΝΕΡΓΟΠΟΙΗΣΗ ∆ΙΑΚΟΠΩΝ ΣΤΗ ΓΡΑΜΜΗ27

  while (getchar() != 'e') {
  }

  aic->IDCR = (1 << PIOA_ID) | (1 << TC0_ID); //∆ΙΑΚΟΠΗΤΩΝ AIC interrupts
  tc->Channel_0.CCR = 0x02; //ΑΠΕΝΕΡΓΟΠΟΙΗΣΗ ΤΟΥ Timer
  CLEANUP;
  return 0;
}

void FIQ_handler(void) {
  unsigned int data_in = 0;
  unsigned int fiq = 0;
  unsigned int data_out;
  fiq = aic->IPR; //ΕΝΤΟΠΙΣΜΟΣ ΠΕΡΙΦΕΡΕΙΑ ΚΟΥ ΠΟΥ ΠΡΟΚΑΛΕΣΕ ΤΗ ∆ΙΑΚΟΠΗ
  if (fiq & (1 << PIOA_ID)) { //ΕΛΕΓΧΟΣ ΓΙΑ PIOA
    data_in = pioa->ISR; //ΕΚΚΑΘΑΡΙΣΗ ΤΗΣ ΠΗΓΗΣ ΤΗΣ ∆ΙΑΚΟΠΗΣ
    aic->ICCR = (1 << PIOA_ID); //ΕΚΚΑΘΑΡΙΣΗ ΤΗΣ ∆ΙΑΚΟΠΗΣ ΑΠΟ AIC
    data_in = pioa->PDSR;      //ΑΝΑΓΝΩΣΗ ΤΙΜΩΝ ΕΙΣΟ∆ΟΥ
    if (data_in & 0x01) { //∆ΙΑΚΟΠΤΗΣ ΠΑΤΗΜΕΝΟΣ;
      if (button_state == BUT_IDLE) {
        button_state = BUT_PRESSED;
        if (led_state == LED_IDLE) { //ΑΝ ∆ΕΝ ΑΝΑΒΟΣΒΗΝΕΙ
          tc->Channel_0.CCR = 0x05;  //ΕΝΑΡΞΗ ΜΕΤΡΗΤΗ
          led_state = LED_FLASHING;
        } else {
          tc->Channel_0.CCR = 0x02; //∆ΙΑΚΟΠΗ ΜΕΤΡΗΤΗ
          led_state = LED_IDLE;
        }
      }
    } else {
      if (button_state == BUT_PRESSED)
        button_state = BUT_IDLE;
    }
  }

  if (fiq & (1 << TC0_ID)) {
    data_out = tc->Channel_0.SR; //ΕΚΚΑΘΑΡΙΣΗ ΤΗΣ ΠΗΓΗΣ ΤΗΣ ∆ΙΑΚΟΠΗΣ
    aic->ICCR = (1 << TC0_ID); //ΕΚΚΑΘΑΡΙΣΗ ∆ΙΑΚΟΠΗΣ ΚΑΙ ΑΠΟ AIC
    data_out = pioa->ODSR; //ΑΝΑΓΝΩΣΗ ΤΙΜΩΝ ΕΞΟ∆ΟΥ
    pioa->SODR = data_out & 0x02;
    pioa->CODR = data_out & 0x02;
    tc->Channel_0.CCR = 0x05;
  }
}
