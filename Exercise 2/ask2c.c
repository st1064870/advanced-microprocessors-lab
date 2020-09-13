#define BUT_PRESSED 0
#define BUT_RELEASED 1
 
 
unsigned int prev_but1_state;
int state;
int game_level;
 
volatile int counter=101;
volatile bool win=false;
void buttonUpdate(){
    if(counter==0){
        win=true;
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    pinMode(5, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(2), buttonUpdate, FALLING);
   
    prev_but1_state = BUT_RELEASED;
 
    game_level=0;
}
 
void loop() {
    int counterr=0;
    for(int i=0;i<3;++i){
        bool result=play_round();
        if(result){
            ++counterr;
        }
    }
    Serial.print(counterr);
}
 
bool play_round() {
    int current_try=0;
    counter=101;
    for(;;){
        generate_pwm(10-game_level,--counter);
        if(win) {
            win=false;
            current_try=0;
            ++game_level;
            return true;
        }
        if(counter<0){
            ++current_try;
            counter=101;
            if(current_try>=3)return false;
        }
    }
}
 
void generate_pwm(int cycles, int duty_cycle) {
    int j;
    for (j=0; j<cycles; j++) {
        int i;
        for (i=0; i<duty_cycle; i++) {
            digitalWrite(5, HIGH);
        }
 
        for (i=duty_cycle; i<100; i++) {
            digitalWrite(5, LOW);
        }
    }
}