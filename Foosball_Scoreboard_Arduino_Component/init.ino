/*
 * Store all initialization functions here.
 */

/*
 * Initialize variables which store game state.
 */
void initGame()
{
  invertedRound = false;
  totalGameSeconds = 0;
  playerData[0] = (player) {0, 0, 0};
  playerData[1] = (player) {0, 0, 0};
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
  delay(1500);
  
  genieBegin(GENIE_SERIAL, 115200);
  genieAttachEventHandler(myEventHandler);
  
  genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
  genieWriteObject(GENIE_OBJ_SOUND, 1, SOUND_VOLUME);
  
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
      if (event.reportObject.index == 0)
      {
        // Reset the game variables.
        initGame();
        
        // Reset the scoreboard digits to 0.
        genieWriteObject(GENIE_OBJ_LED_DIGITS, BLACK_SCORE_DIGIT_IDX, 0);
        genieWriteObject(GENIE_OBJ_LED_DIGITS, YELLOW_SCORE_DIGIT_IDX, 0);
        genieWriteObject(GENIE_OBJ_LED_DIGITS, BLACK_ROUND_DIGIT_IDX, 0);
        genieWriteObject(GENIE_OBJ_LED_DIGITS, YELLOW_ROUND_DIGIT_IDX, 0);
        genieWriteObject(GENIE_OBJ_LED_DIGITS, CLOCK_MINUTES_IDX, 0);
        genieWriteObject(GENIE_OBJ_LED_DIGITS, CLOCK_SECONDS_IDX, 0);
        
        genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
        genieWriteStr(0, "Ready");
      }
    }
  }
}
