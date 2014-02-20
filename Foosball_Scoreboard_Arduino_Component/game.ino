/*
 * Holds game related functions.
 */
 
/**
 * Updates the match scoreboard at the bottom of the screen after a player wins a match. ONLY call this
 * when somebody wins the match.
 */
void updateMatchScores()
{
  int colorWinner;
  
  // Figure out who won.
  if (playerData[0].matchScore == POINTS_PER_MATCH)
  {
    playerData[0].matchesWon++;
    
    if (invertedRound) colorWinner = YELLOW;
    else colorWinner = BLACK;
  }
  else
  {
    playerData[1].matchesWon++;
    
    if (invertedRound) colorWinner = BLACK;
    else colorWinner = YELLOW;
  }
  
  // Update the scoreboard to reflect the end of this match.
  if (!invertedRound)
  {
    // Player 0 is on left, player 1 is on right.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[0].matchesWon);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[1].matchesWon);
  }
  else
  {
    // Player 1 is on right, player 0 is on left.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[1].matchesWon);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[0].matchesWon);
  }
  
  // End the tournament if somebody won.
  if (playerData[0].matchesWon == TOURNAMENT_WIN_LIMIT || playerData[1].matchesWon == TOURNAMENT_WIN_LIMIT)
  {
    endTournament();
    return;
  }
  
  // Play the match winning sound.
  genieWriteObject(GENIE_OBJ_SOUND, 0, 1);
  delay(AFTER_ROUND_PAUSE_MSEC);
  
  // Show a swapped match score for when the players switch sides.
  if (invertedRound)
  {
    // Player A is on left, player B is on right.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[0].matchesWon);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[1].matchesWon);
  }
  else
  {
    // Player A is on right, player B is on left.
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[1].matchesWon);
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[0].matchesWon);
  }
  
  // Tell the player who should put the ball in.
  if (colorWinner == YELLOW)
  {
    genieWriteStr(0, "Ready.\nYellow, put in ball.");
  }
  else
  {
    genieWriteStr(0, "Ready.\nBlack, put in ball.");
  }
  
  // Clear the main scoreboard.
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, 0);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, 0);
  playerData[0].matchScore = 0;
  playerData[1].matchScore = 0;
  invertedRound = !invertedRound;
}

/*
 * Called when a player reaches the tournament limit.
 */
void endTournament()
{
  genieWriteStr(0, "Good game!");
  
  // Play the tournament win sound & show the stats form.
  genieWriteObject(GENIE_OBJ_SOUND, 0, 2);
  genieWriteObject(GENIE_OBJ_FORM, 1, 0);
}
