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
boolean ballInPlay = false;
boolean invertedRound = false;           // Set to false when player 0 is yellow. Set to true when player 0 is black.
boolean activeGame = false;              // Set to true when a game is in progress. False when the menus are being navigated.
int totalGameSeconds = 0;

/* Clock variables / config */
const long UPDATE_CLOCK_CYCLES = 10000;  // Check for the update clock flag every 10,000 cycles.
long loopCycleCount = 0;
volatile boolean updateClockFlag = false;

/* Game config variables. */
int POINTS_PER_MATCH = 5;
int TOURNAMENT_WIN_LIMIT = 3;            // How many games a player has to win before winning the tournament.
const int AFTER_ROUND_PAUSE_MSEC = 7000; // The amount of time to freeze the scoreboard after each round, in milliseconds.
const int SOUND_VOLUME = 127;            // The volume of the sounds. Range [0, 127]

/* LCD config variables. */
const int YELLOW_SCORE_DIGIT_IDX = 0;
const int BLACK_SCORE_DIGIT_IDX = 1;
const int YELLOW_ROUND_DIGIT_IDX = 2;
const int BLACK_ROUND_DIGIT_IDX = 3;
const int CLOCK_MINUTES_IDX = 4;
const int CLOCK_SECONDS_IDX = 5;
const int BTN_PLAY_AGAIN_IDX = 0;
const int BTN_MAIN_MENU_IDX = 1;
const int BTN_QUICK_MATCH_IDX = 2;
const int BTN_QUICK_TOURNAMENT_IDX = 3;
const int BTN_CUSTOM_MATCH_IDX = 4;
const int BTN_CUSTOM_TOURNAMENT_IDX = 5;

/* Enums */
const int YELLOW = 0;
const int BLACK = 1;

typedef struct
{
  int score;
  int match_win;
  int tournament_win;
  int ks_15_killjoy;
  int ks_14_extermination;
  int ks_13_un_friggin_believable;
  int ks_12_inconceivable;
  int ks_11_invincible;
  int ks_10_untoucable;
  int ks_9_rampage;
  int ks_8_running_riot;
  int ks_7_killing_frenzy;
  int ks_6_killing_spree;
  int ks_5_killtacular;
  int ks_4_overkill;
  int ks_3_triple_kill;
  int ks_2_double_kill;
  int error;
} sounds;

sounds soundMap;

typedef struct
{
  int matchScore;
  int totalScore;
  int matchesWon;
  int streak;
  int longestStreak;
} player;

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
  
  // Initialize the sound map.
  soundMap.score = 0;
  soundMap.match_win = 1;
  soundMap.tournament_win = 2;
  soundMap.ks_15_killjoy = 3;
  soundMap.ks_14_extermination = 4;
  soundMap.ks_13_un_friggin_believable = 5;
  soundMap.ks_12_inconceivable = 6;
  soundMap.ks_11_invincible = 7;
  soundMap.ks_10_untoucable = 8;
  soundMap.ks_9_rampage = 9;
  soundMap.ks_8_running_riot = 10;
  soundMap.ks_7_killing_frenzy = 11;
  soundMap.ks_6_killing_spree = 12;
  soundMap.ks_5_killtacular = 13;
  soundMap.ks_4_overkill = 14;
  soundMap.ks_3_triple_kill = 15;
  soundMap.ks_2_double_kill = 16;
  soundMap.error = 17;

  initGame();
  initDisplay();
}

void loop()
{
  // Either poll for a score or a ball in play, depending on whether or not a ball is in play.
  if (activeGame)
  {
    if (ballInPlay) pollForScore();
    else pollForBallInsert();
  }

  // Receive touch events from LCD panel.
  genieDoEvents();
  
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
      genieWriteObject(GENIE_OBJ_SOUND, 0, soundMap.error);
      
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
      ballInPlay = true;
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

