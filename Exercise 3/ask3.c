volatile int pressed=false;
volatile int buttCounter=0;
void buttonUpdate(){
  pressed=!digitalRead(2);
  buttCounter+=pressed;
}
char resetMe=false;
void pollIO(){
  static unsigned int lastUnpressed=0;
  if(pressed){
    if(millis()-lastUnpressed>=100){
      resetMe=true;
    }
  }else{
    lastUnpressed=millis();
	resetMe=false;
  }
}

extern volatile unsigned long timer0_millis;
void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);	
  pinMode(3, OUTPUT);		
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  
  attachInterrupt(digitalPinToInterrupt(2),buttonUpdate,CHANGE);
}

unsigned int numCounter=0;
unsigned int numCounter2Pause=0;
unsigned int SDnumCounter=0;
unsigned int yeNo2blink=0;
unsigned int yeNo2blinkCopy=0;
void reset(){
  numCounter=0;
  SDnumCounter=0;
  yeNo2blink=0;
  yeNo2blinkCopy=0;
  noInterrupts ();
  buttCounter=0;
  pressed=false;
  timer0_millis = 0;
  interrupts ();
  resetMe=false;
  numCounter2Pause=0;
}

void loop(){
  pollIO();
  while(timer0_millis<100*numCounter){
    if(!(buttCounter%2)){
      digitalWrite(3,HIGH);
      if(yeNo2blinkCopy)digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
    }else{
      digitalWrite(5,HIGH);
    }
  }
  ++numCounter;
  
  if(yeNo2blinkCopy)--yeNo2blinkCopy;
  while(timer0_millis<100*numCounter){
    if(!(buttCounter%2)){
	  digitalWrite(3,LOW);
      digitalWrite(4,LOW);
      digitalWrite(5,LOW);
    }else{
      digitalWrite(5,HIGH);
    }
  }
  ++numCounter;
  if(numCounter%10==0){
    yeNo2blink=numCounter/10;
    yeNo2blinkCopy=yeNo2blink;
  }
  
  if(numCounter>=59||resetMe)reset();
  if(!(buttCounter%2)){
    numCounter2Pause=numCounter;
  }
  char str[64];
  sprintf(str, "counter:%d\n",numCounter2Pause);
  Serial.print(str);
}