void setup() {
  // Serial.begin(9600);
  for (int i = 11; i > 1; --i)
    pinMode(i, OUTPUT);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
}

bool butIsPressed[4] = {0, 0, 0, 0}; // 0 1 12 13
bool butJustPressed[4] = {
    0, 0, 0, 0}; // 0 1 12 13 //use this and not butIsPressed to make it harder

void pollIO() {
  static bool butPrevPressed[4] = {0, 0, 0, 0}; // for pio: 0 1 12 13

  butPrevPressed[0] = butIsPressed[0];
  butPrevPressed[1] = butIsPressed[1];
  butPrevPressed[2] = butIsPressed[2];
  butPrevPressed[3] = butIsPressed[3];
  butIsPressed[0] = !digitalRead(0);
  butIsPressed[1] = !digitalRead(1);
  butIsPressed[2] = !digitalRead(12);
  butIsPressed[3] = /*!!!!!!!!!*/ digitalRead(13); // arduinos lazy pullup
  butJustPressed[0] = !butPrevPressed[0] && butIsPressed[0] ? 1 : 0;
  butJustPressed[1] = !butPrevPressed[1] && butIsPressed[1] ? 1 : 0;
  butJustPressed[2] = !butPrevPressed[2] && butIsPressed[2] ? 1 : 0;
  butJustPressed[3] = !butPrevPressed[3] && butIsPressed[3] ? 1 : 0;
}

static unsigned short const speedUp =
    10; // to comprehend for tinkercad simulation lag
static unsigned short const speedUp2 =
    2; // to comprehend for tinkercad simulation lag
static unsigned short baseBallFreq = 1 * speedUp; // hz
static short scoreDifference = 0; // left player wins[-2...+2]right wins
static bool starting = true;

void drawBall(float ballX) {         // ballX: [0..10]
  digitalWrite(round(ballX) + 2, 1); //+2 offset for convenience
}

void clearCanvas() {
  for (int i = 11; i > 1; --i)
    digitalWrite(i, 0);
}

void tickGame() {
  static float ballX = 0.0f; // right player plays 1st
  static float ballVel = 0.0f;
  static int t1 = 0;

  static bool LboostUsed = false;
  static bool RboostUsed = false;
  //
  if (starting && 0) // remove if
    if (butJustPressed[0]) {
      ballVel = baseBallFreq;
      starting = 0;
    } else if (butJustPressed[1]) {
      ballVel = baseBallFreq * 5;
      starting = 0;
    }
  ballX += ballVel * ((millis() - t1) / 1000.0f);
  t1 = millis();
  clearCanvas();
  if (ballX <= 9 && ballX >= 0) { // inside of bounds
    drawBall(ballX);
    if (ballX <= 4.5) {
      LboostUsed = false;
      if (butJustPressed[0] && !RboostUsed) {
        ballVel = abs(ballVel) >= .4 ? abs(baseBallFreq * 5 * ballVel)
                                     : baseBallFreq * 5;
        RboostUsed = true;
      }
      if (butJustPressed[1]) {
        ballVel = baseBallFreq;
      }
    } else {
      RboostUsed = false;
      if (butJustPressed[3] && !LboostUsed) {
        ballVel = abs(ballVel) >= .4 ? -abs(baseBallFreq * 5 * ballVel)
                                     : -(float)baseBallFreq * 5;
        LboostUsed = false;
      }
      if (butJustPressed[2]) {
        ballVel = -(float)baseBallFreq;
      }
    }
  } else {
    {
      int s1 = millis();
      int temp;
      while ((temp = (millis() - s1)) / 1000.0f <= 1.2f / speedUp2) {
        (ballX > 9)
            ? digitalWrite(11, temp % (200 / speedUp2) > 100 / speedUp2)
            : digitalWrite(
                  2, temp % (200 / speedUp2) >
                         100 / speedUp2); // play animation for goal receiving
      }
    }
    LboostUsed = RboostUsed = false;
    ballVel = 0;
    if (ballX > 9) {
      ballX = 9;
      ++scoreDifference;
      if (scoreDifference == 2) {
        scoreDifference = 0;
        // right wins
        int s1 = millis();
        int temp;
        while ((temp = (millis() - s1)) / 1000.0f <=
               3.0f / speedUp2) { // winning animation for 3 secs
          for (int i = 2; i <= 6; ++i) {
            digitalWrite(i, (temp % (150 / speedUp2) > 75 / speedUp2));
          }
        }
      }
    } else {
      ballX = 0;
      --scoreDifference;
      if (scoreDifference == -2) {
        scoreDifference = 0;
        // right wins
        int s1 = millis();
        int temp;
        while ((temp = (millis() - s1)) / 1000.0f <=
               3.0f / speedUp2) { // winning animation for 3 secs
          for (int i = 7; i <= 11; ++i) {
            digitalWrite(i, (temp % (150 / speedUp2) > 75 / speedUp2));
          }
        }
      }
    }
  }
}

void loop() {
  pollIO();
  tickGame();
}