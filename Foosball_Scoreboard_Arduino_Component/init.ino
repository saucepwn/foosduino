/*
 * Store all initialization functions here.
 */

/*
 * Initialize variables which store game state.
 */
void initGame()
{
  invertedRound = false;
  playerData[0] = (player) {0, 0, 0};
  playerData[1] = (player) {0, 0, 0};
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
  
  genieWriteObject(GENIE_OBJ_FORM, 0, 0);  // Select form 0 (the scoreboard).
  genieWriteObject(GENIE_OBJ_SOUND, 1, SOUND_VOLUME);
  
  genieWriteStr(0, "Ready");
}
