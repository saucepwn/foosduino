#include <genieArduino.h>

/* Define pins */
const int blackScorePin = 8;
const int yellowScorePin = 9;
const int ballInsertPin = 10;
const int ledPin =  13;

// Game status variables.
int blackScore = 0;
int yellowScore = 0;
int playerARounds = 0;
int playerBRounds = 0;
int ballInPlay = 0;

// Game config variables.
const int POINTS_PER_MATCH = 5;

void setup()
{
  // Set up the ball score switches to be inputs:
  pinMode(blackScorePin, INPUT_PULLUP);
  pinMode(yellowScorePin, INPUT_PULLUP);
  pinMode(ballInsertPin, INPUT_PULLUP);
 
  // Set up the LED pin to be an output:
  pinMode(ledPin, OUTPUT);
  
  // Set up the display.
  initDisplay();
}
 
void loop()
{ 
  if (digitalRead(ballInsertPin) == LOW && ballInPlay == 0)
  {
    ballInPlay = 1;
    genieWriteStr(0, "Ball is in play");
    digitalWrite(ledPin, HIGH);
  }
  
  if (ballInPlay == 1) pollForScore();
}

void pollForScore()
{
  if (digitalRead(blackScorePin) == LOW)
  {
    blackScore++;
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, blackScore);
    if (blackScore != POINTS_PER_MATCH) genieWriteStr(0, "Black scores!");

    ballInPlay = 0;
  }
  else if (digitalRead(yellowScorePin) == LOW)
  {
    yellowScore++;
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, yellowScore);
    if (yellowScore != POINTS_PER_MATCH) genieWriteStr(0, "Yellow scores!");
    
    ballInPlay = 0;
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
  delay(9000);
  
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

void initDisplay()
{
  genieBegin (GENIE_SERIAL, 115200);
  //genieAttachEventHandler(myEventHandler);
  
  // ResetSend a reset signal to the display then wait for it to come up.
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);
  delay(100);
  digitalWrite(4, 0);
  delay(3000);
  
  genieWriteStr(0, "Ready");
  genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
}
