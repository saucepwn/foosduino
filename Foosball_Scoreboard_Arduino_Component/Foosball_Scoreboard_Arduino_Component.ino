#include <genieArduino.h>

/* Define pins */
const int clockInterrupt = 0;  // For Arduino Uno, interrupt 0 is pin 2.
const int timerOutputPin = 3;  // A PWM pin which will generate a square wave for our timer.
const int displayResetPin = 4;
const int blackScorePin = 8;
const int yellowScorePin = 9;
const int ballInsertPin = 10;
const int ledPin =  13;

/* Game status variables. */
int blackScore = 0;
int yellowScore = 0;
int playerARounds = 0;
int playerBRounds = 0;
int ballInPlay = 0;

/* Clock variables / config */
const long UPDATE_CLOCK_CYCLES = 10000;      // Check for the update clock flag every 10,000 cycles.
long loopCycleCount = 0;
volatile boolean updateClockFlag = false;
volatile int clockSeconds;
volatile int clockMinutes;
int pulseCount;

/* Game config variables. */
const int POINTS_PER_MATCH = 5;
const int AFTER_ROUND_PAUSE_MSEC = 9000;    // The amount of time to freeze the scoreboard after each round, in milliseconds.

void setup()
{
  // Set up the ball score switches to be inputs:
  pinMode(blackScorePin, INPUT_PULLUP);
  pinMode(yellowScorePin, INPUT_PULLUP);
  pinMode(ballInsertPin, INPUT_PULLUP);
   
  // Set up the LED pin to be an output:
  pinMode(ledPin, OUTPUT);
  
  initDisplay();
}
 
void loop()
{
  // Either poll for a score or a ball in play, depending on whether or not a ball is in play.
  if (ballInPlay == 0) pollForBallInsert();
  else pollForScore();
  
  if (loopCycleCount == UPDATE_CLOCK_CYCLES)
  {
    if (updateClockFlag) updateClockDisplay();
    loopCycleCount = 0;
  }
  loopCycleCount++;
}

/**
 * Poll the ball insert pin which would mean a ball was inserted into play. Assumes
 * a ball is not currently in play.
 */
void pollForBallInsert()
{
  if (digitalRead(ballInsertPin) == LOW)
  {
    ballInPlay = 1;
    genieWriteStr(0, "Ball is in play");
    startClock(true);
  }
}

/**
 * Poll the scoreboard pins for input which would mean a player has scored.
 */
void pollForScore()
{
  if (digitalRead(blackScorePin) == LOW)
  {
    stopClock(false);
    ballInPlay = 0;
    
    blackScore++;
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, blackScore);
    if (blackScore != POINTS_PER_MATCH) genieWriteStr(0, "Black scores!");
  }
  else if (digitalRead(yellowScorePin) == LOW)
  {
    stopClock(false);
    ballInPlay = 0;
    
    yellowScore++;
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, yellowScore);
    if (yellowScore != POINTS_PER_MATCH) genieWriteStr(0, "Yellow scores!");
  }
  
  // Check to see if the match is over.
  if (blackScore == POINTS_PER_MATCH)
  {
    genieWriteStr(0, "Black wins this round!\nPlease switch sides.");
    updateMatchScores(1);
  }
  else if (yellowScore == POINTS_PER_MATCH)
  {
    genieWriteStr(0, "Yellow wins this round!\nPlease switch sides.");
    updateMatchScores(0);
  }
}

/**
 * Updates the match scoreboard at the bottom of the screen after a player wins a match.
 * @param winningTeam The team that just won. 0 for yellow, 1 for black.
 */
void updateMatchScores(int winningTeam)
{
  int totalRoundsPlayed = playerARounds + playerBRounds;
  boolean oddRound = (boolean) (totalRoundsPlayed % 2);
  
  if (!oddRound && winningTeam == 0 || oddRound && winningTeam == 1)
  {
    playerARounds++;
  }
  else
  {
    playerBRounds++;
  }
  
  // Update the scoreboard to reflect the end of this match.
  if (!oddRound)
  {
    // Player A is on left, player B is on right.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerARounds);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerBRounds);
  }
  else
  {
    // Player A is on right, player B is on left.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerBRounds);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerARounds);
  }
  
  // Give teams a chance to switch sides.
  delay(AFTER_ROUND_PAUSE_MSEC);
  
  // Show a swapped match score for when the players switch sides.
  if (oddRound)
  {
    // Player A is on left, player B is on right.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerARounds);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerBRounds);
  }
  else
  {
    // Player A is on right, player B is on left.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerBRounds);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerARounds);
  }
  
  // Clear the main scoreboard.
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, 0);
  yellowScore = 0;
  blackScore = 0;
  
  // Tell the player who should put the ball in.
  if (winningTeam == 0)
  {
    genieWriteStr(0, "Ready.\nYellow, put in ball.");
  }
  else
  {
    genieWriteStr(0, "Ready.\nBlack, put in ball.");
  }
}

/**
 * Starts a 50% duty cycle on a PWM pin.
 */
void startClock(boolean reset)
{
  if (reset)
  {
    clockSeconds = 0;
    clockMinutes = 0;
    updateClockDisplay();
  }

  pulseCount = 0;  
  attachInterrupt(clockInterrupt, clockTick, RISING);
  analogWrite(timerOutputPin, 127);
}

/**
 * Called as an Interrupt Handler Routine on each clock pulse. Update the clock
 * on the display if one second has elapsed.
 */
void clockTick()
{
  pulseCount++;
  if (pulseCount == 489)  // 490Hz reached
  {
    clockSeconds++;
    if (clockSeconds == 60)
    {
      clockMinutes++;
      clockSeconds = 0;
    }
    
    pulseCount = 0;
    updateClockFlag = true;
  }
}

/**
 * Updates the clock on the display.
 */
void updateClockDisplay()
{
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 4, clockMinutes);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 5, clockSeconds);
  updateClockFlag = false;
}

/**
 * Stop the clock from ticking.
 * @param reset True to reset the digits on the clock.
 */
void stopClock(boolean reset)
{
  detachInterrupt(clockInterrupt);
  analogWrite(timerOutputPin, 0);
  
  if (reset)
  {
    clockSeconds = 0;
    clockMinutes = 0;
    updateClockDisplay();
  }
  
  pulseCount = 0;
}

/**
 * Initializes the Genie LCD display.
 */
void initDisplay()
{
  genieBegin (GENIE_SERIAL, 115200);
  //genieAttachEventHandler(myEventHandler);
  
  // ResetSend a reset signal to the display then wait for it to come up.
  pinMode(displayResetPin, OUTPUT);
  digitalWrite(displayResetPin, 1);
  delay(100);
  digitalWrite(displayResetPin, 0);
  delay(3000);
  
  genieWriteStr(0, "Ready");
  genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
}
