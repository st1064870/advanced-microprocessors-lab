#define BUT_PRESSED 0
#define BUT_RELEASED 1

unsigned int prev_but1_state;
unsigned int prev_but2_state;
int counter;

void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(5, OUTPUT);

    prev_but1_state = BUT_RELEASED;
    prev_but2_state = BUT_RELEASED;
    counter = 50;
}

void loop() {
    unsigned int but1_state = digitalRead(2);
    if (but1_state == BUT_RELEASED && prev_but1_state == BUT_PRESSED) {
        counter+=10;
    }

    unsigned int but2_state = digitalRead(3);
    if (but2_state == BUT_RELEASED && prev_but2_state == BUT_PRESSED) {
        counter-=10;
    }
    
    prev_but1_state = but1_state;
    prev_but2_state = but2_state;

    if (counter > 100) {
        counter = 100;
    }

    if (counter < 0) {
        counter = 0;
    }

    analogWrite(5, (int)(counter * 2.55));
    Serial.println(counter * 2.55);
}