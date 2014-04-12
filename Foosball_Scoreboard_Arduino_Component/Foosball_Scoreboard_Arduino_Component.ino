#include <genieArduino.h>

/* Define pins */
const int clockInterrupt = 0;  // For Arduino Uno, interrupt 0 is pin 2.
const int timerOutputPin = 3;  // A PWM pin which will generate a square wave for our timer.
const int displayResetPin = 4;
const int yellowInsertLightPin = 5;
const int blackInsertLightPin = 6;
const int blackScorePin = 8;
const int yellowScorePin = 9;
const int ballInsertPin = 10;
const int ledPin =  13;

/* Ball insert (beam break sensors) variables */
const int ballInsert1 = A5;
const int ballInsert2 = A4;
const int ballInsertThreshold = 250;
boolean insertInProgress = false;

/* Game status variables. */
int ballInPlay = 0;
boolean invertedRound = false;               // Set to false when player 0 is yellow. Set to true when player 0 is black.
int totalGameSeconds = 0;

/* Clock variables / config */
const long UPDATE_CLOCK_CYCLES = 10000;      // Check for the update clock flag every 10,000 cycles.
long loopCycleCount = 0;
volatile boolean updateClockFlag = false;

/* Game config variables. */
const int POINTS_PER_MATCH = 8;
const int TOURNAMENT_WIN_LIMIT = 1;          // How many games a player has to win before winning the tournament.
const int AFTER_ROUND_PAUSE_MSEC = 1000;     // The amount of time to freeze the scoreboard after each round, in milliseconds.
const int SOUND_VOLUME = 126;                // The volume of the sounds. Range [0, 127]

/* Enums */
const int YELLOW = 0;
const int BLACK = 1;

typedef struct
{
  int matchScore;
  int totalScore;
  int matchesWon;
} 
player;

player playerData[2];

void setup()
{
  // Set up the ball score switches to be inputs:
  pinMode(blackScorePin, INPUT_PULLUP);
  pinMode(yellowScorePin, INPUT_PULLUP);
  pinMode(ballInsertPin, INPUT_PULLUP);

  // Set up the LED pins to be an output:
  pinMode(yellowInsertLightPin, OUTPUT);
  pinMode(blackInsertLightPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Set up ball insert pins (beam break sensors)
  pinMode(ballInsert1, INPUT);
  pinMode(ballInsert2, INPUT);

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
  int ballInsert1Value = analogRead(ballInsert1);
  int ballInsert2Value = analogRead(ballInsert2);
  
  if (false == insertInProgress && (ballInsert1Value > ballInsertThreshold || ballInsert2Value > ballInsertThreshold))
  {
    // Detect when the beam has been broken for either ball insert sensor.
    insertInProgress = true;
  }
  
  if (digitalRead(ballInsertPin) == LOW || (insertInProgress && ballInsert1Value < ballInsertThreshold && ballInsert2Value < ballInsertThreshold))
  {
    insertInProgress = false;
    
    // Make sure a ball isn't stuck in the goal.
    if (digitalRead(blackScorePin) == LOW || digitalRead(yellowScorePin) == LOW)
    {
      // Show the ball stuck warning.
      genieWriteObject(GENIE_OBJ_FORM, 2, 0);
      genieWriteObject(GENIE_OBJ_SOUND, 0, 1);
      
      // Trap execution here until the ball gets unstuck.
      while (true)
      {
        if (digitalRead(blackScorePin) == HIGH && digitalRead(yellowScorePin) == HIGH)
        {
          genieWriteObject(GENIE_OBJ_FORM, 0, 0);
          genieWriteStr(0, "Place ball in play.");
          break;
        }
      }
    }
    else
    {
      ballInPlay = 1;
      genieWriteStr(0, "Ball is in play");
      digitalWrite(yellowInsertLightPin, LOW);
      digitalWrite(blackInsertLightPin, LOW);
  
      startClock(true);
    }
  }
}

/**
 * Poll the scoreboard pins for input which would mean a player has scored.
 */
void pollForScore()
{
  if (digitalRead(blackScorePin) == LOW)
  {
    registerScore(BLACK);
  }
  else if (digitalRead(yellowScorePin) == LOW)
  {
    registerScore(YELLOW);
  }
}

