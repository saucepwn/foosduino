/*
 * Store all initialization functions here.
 */

/*
 * Initialize variables which store game state.
 */
void initGame()
{
  invertedRound = false;
  activeGame = true;
  totalGameSeconds = 0;
  insertInProgress = false;
  playerData[0] = (player) {0, 0, 0, 0, 0};
  playerData[1] = (player) {0, 0, 0, 0, 0};
}

/**
 * Initializes the Genie LCD display.
 */
void initDisplay()
{
  // Wait for the screen to power up.
  delay(1000);
  
  // ResetSend a reset signal to the display then wait for it to come up.
  pinMode(displayResetPin, OUTPUT);
  digitalWrite(displayResetPin, 1);
  delay(100);
  digitalWrite(displayResetPin, 0);

  delay(1000);  
  genieBegin(GENIE_SERIAL, 115200);
  delay(1000);
  
  genieAttachEventHandler(myEventHandler);
  
  genieWriteObject(GENIE_OBJ_FORM, 3, 0);  // Select form 3 (the main menu).
  genieWriteObject(GENIE_OBJ_SOUND, 1, SOUND_VOLUME);
}

/**
 * Resets all digits on the scoreboard form to 0 and then displays the scoreboard form.
 */
void resetScoreboard()
{
  genieWriteObject(GENIE_OBJ_LED_DIGITS, BLACK_SCORE_DIGIT_IDX, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, YELLOW_SCORE_DIGIT_IDX, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, BLACK_ROUND_DIGIT_IDX, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, YELLOW_ROUND_DIGIT_IDX, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, CLOCK_MINUTES_IDX, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, CLOCK_SECONDS_IDX, 0);
  
  genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
  genieWriteStr(0, "Ready");
}

void myEventHandler()
{
  genieFrame event;
  genieDequeueEvent(&event);
  
  if (event.reportObject.cmd == GENIE_REPORT_EVENT)
  {
    // Handle button presses.
    if (event.reportObject.object == GENIE_OBJ_WINBUTTON)
    {
      // The "play again" button was pressed.
      if (event.reportObject.index == BTN_PLAY_AGAIN_IDX)
      {
        // Reset the game variables.
        initGame();
        resetScoreboard();
      }
      
      // The "main menu" button was pressed.
      if (event.reportObject.index == BTN_MAIN_MENU_IDX)
      {
        // Go to the main menu screen.
        genieWriteObject(GENIE_OBJ_FORM, 3, 0);  // Select form 3 (the main menu).
      }
      
      // The "quick match" button was pressed.
      if (event.reportObject.index == BTN_QUICK_MATCH_IDX)
      {
        // Set the game up for a single round up to 8 points.
        POINTS_PER_MATCH = 8;
        TOURNAMENT_WIN_LIMIT = 1;
        
        // Reset the game variables.
        initGame();
        resetScoreboard();
      }
      
      // The "quick tournament" button was pressed.
      if (event.reportObject.index == BTN_QUICK_TOURNAMENT_IDX)
      {
        // Set up the game for a tournament, best of 5, 5 points per match.
        POINTS_PER_MATCH = 5;
        TOURNAMENT_WIN_LIMIT = 3;
        
        // Reset the game variables.
        initGame();
        resetScoreboard();
      }
    }
  }
}
