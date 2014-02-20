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
int playerARounds = 0;
int playerBRounds = 0;
int ballInPlay = 0;
boolean invertedRound = false;                       // Set to false when player 0 is yellow. Set to true when player 0 is black.

/* Clock variables / config */
const long UPDATE_CLOCK_CYCLES = 10000;      // Check for the update clock flag every 10,000 cycles.
long loopCycleCount = 0;
volatile boolean updateClockFlag = false;

/* Game config variables. */
const int POINTS_PER_MATCH = 5;
const int TOURNAMENT_WIN_LIMIT = 2;          // How many games a player has to win before winning the tournament.
const int AFTER_ROUND_PAUSE_MSEC = 9000;     // The amount of time to freeze the scoreboard after each round, in milliseconds.
const int SOUND_VOLUME = 127;                // The volume of the sounds. Range [0, 127]

/* Enums */
const int YELLOW = 0;
const int BLACK = 1;

typedef struct
{
  int matchScore;
  int totalScore;
  int matchesWon;
} player;

player playerData[2];

void setup()
{
  // Set up the ball score switches to be inputs:
  pinMode(blackScorePin, INPUT_PULLUP);
  pinMode(yellowScorePin, INPUT_PULLUP);
  pinMode(ballInsertPin, INPUT_PULLUP);
   
  // Set up the LED pin to be an output:
  pinMode(ledPin, OUTPUT);
  
  initGame();
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
  int score;
  
  if (digitalRead(blackScorePin) == LOW)
  {
    stopClock(false);
    ballInPlay = 0;
    
    score = ++playerData[ getArrayIndexForColor(invertedRound, BLACK) ].matchScore;
    
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, score);
    if (score != POINTS_PER_MATCH) genieWriteStr(0, "Black scores!");
  }
  else if (digitalRead(yellowScorePin) == LOW)
  {
    stopClock(false);
    ballInPlay = 0;
    
    score = ++playerData[ getArrayIndexForColor(invertedRound, YELLOW) ].matchScore;
    
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, score);
    if (score != POINTS_PER_MATCH) genieWriteStr(0, "Yellow scores!");
  }
  else
  {
    // Nobody has scored, return since the following code should only be ran if a player wins.
    return;
  }
  
  // Check to see if the match is over.
  if (playerData[0].matchScore == POINTS_PER_MATCH || playerData[1].matchScore == POINTS_PER_MATCH)
  {
    genieWriteStr(0, "Round over!\nPlease switch sides.");
    updateMatchScores();
  }
  else
  {
    // Index 0 means play sound, sound 0 is the goal score sound.
    genieWriteObject(GENIE_OBJ_SOUND, 0, 0);
  }
}
