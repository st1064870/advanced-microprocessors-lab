#define BUT_PRESSED 0
#define BUT_RELEASED 1

unsigned int prev_but1_state;
int counter;
int state;

void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    pinMode(5, OUTPUT);

    state = 1;
    prev_but1_state = BUT_RELEASED;
    counter = 0;
}

void loop() {
    unsigned int but1_state = digitalRead(2);
    if (but1_state == BUT_RELEASED && prev_but1_state == BUT_PRESSED) {
        state = !state;
    }

    prev_but1_state = but1_state;

    if (state == 0) {
        analogWrite(5, (int)(pow((cos(counter)/2 + 0.6),.8) * 255));
        Serial.println((int)(pow((cos(counter)/2 + 0.6),.8) * 255));
        counter+=0.05;
    }
}