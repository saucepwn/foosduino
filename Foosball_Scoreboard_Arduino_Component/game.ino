/*
 * Holds game related functions.
 */

/**
 * Call when a player scores a goal.
 * @param color: Either YELLOW or BLACK.
 */
void registerScore(int color)
{
  stopClock(false);
  ballInPlay = 0;
  int score = ++playerData[ getArrayIndexForColor(invertedRound, color) ].matchScore;
  
  if (color == BLACK)
  {
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 1, score);
    
    // Light up the ball insert pin for the team that was just scored on.
    if (score != POINTS_PER_MATCH)
    {
      genieWriteStr(0, "Black scores!");
      digitalWrite(yellowInsertLightPin, HIGH);
    }
  }
  else if (color == YELLOW)
  {
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 0, score);
    
    // Light up the ball insert pin for the team that was just scored on.
    if (score != POINTS_PER_MATCH)
    {
      genieWriteStr(0, "Yellow scores!");
      digitalWrite(blackInsertLightPin, HIGH);
    }
  }
  else
  {
    return;  // Invalid input.
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
    
    if (invertedRound) colorWinner = BLACK;
    else colorWinner = YELLOW;
  }
  else
  {
    playerData[1].matchesWon++;
    
    if (invertedRound) colorWinner = YELLOW;
    else colorWinner = BLACK;
  }
  
  // Update the scoreboard to reflect the end of this match.
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[ getArrayIndexForColor(invertedRound, YELLOW) ].matchesWon);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[ getArrayIndexForColor(invertedRound, BLACK) ].matchesWon);
  
  playerData[0].totalScore += playerData[0].matchScore;
  playerData[1].totalScore += playerData[1].matchScore;
  
  // Play the match winning sound.
  genieWriteObject(GENIE_OBJ_SOUND, 0, 2);
  delay(AFTER_ROUND_PAUSE_MSEC);
  
  // End the tournament if somebody won.
  if (playerData[0].matchesWon == TOURNAMENT_WIN_LIMIT || playerData[1].matchesWon == TOURNAMENT_WIN_LIMIT)
  {
    endTournament();
    return;
  }
  
  // Show a swapped match score for when the players switch sides.
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 2, playerData[ getArrayIndexForColor(invertedRound, BLACK) ].matchesWon);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 3, playerData[ getArrayIndexForColor(invertedRound, YELLOW) ].matchesWon);
  
  // Tell the player who should put the ball in.
  if (colorWinner == YELLOW)
  {
    genieWriteStr(0, "Ready.\nYellow, put in ball.");
    digitalWrite(yellowInsertLightPin, HIGH);
  }
  else
  {
    genieWriteStr(0, "Ready.\nBlack, put in ball.");
    digitalWrite(blackInsertLightPin, HIGH);
  }
  
  // Clear the scores.
  for (int i = 0; i < 2; i++)
  {
    genieWriteObject(GENIE_OBJ_LED_DIGITS, i, 0);
    playerData[i].matchScore = 0;
  }
  
  invertedRound = !invertedRound;
}

/**
 * Gets the array index for the given color and game state.
 * @param inverted: Whether or not the match is inverted (pass invertedRound here).
 * @param color: The color to get the index for. YELLOW or BLACK are valid constants here.
 */
int getArrayIndexForColor(boolean inverted, int color)
{
  // For non-side-switching.
  if (color == BLACK) return 1;
  else return 0;
  /*
  if ((inverted && color == YELLOW) || (!inverted && color == BLACK))
  {
    return 1;
  }
  else
  {
    return 0;
  }*/
}

/*
 * Called when a player reaches the tournament limit.
 */
void endTournament()
{
  genieWriteStr(0, "Good game!");
  
  // Play the tournament win sound & show the stats form.
  genieWriteObject(GENIE_OBJ_SOUND, 0, 3);
  
  // Populate & show the postgame stats form.
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 6, playerData[ getArrayIndexForColor(invertedRound, YELLOW) ].totalScore);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 7, playerData[ getArrayIndexForColor(invertedRound, BLACK) ].totalScore);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 8, playerData[ getArrayIndexForColor(invertedRound, YELLOW) ].matchesWon);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 9, playerData[ getArrayIndexForColor(invertedRound, BLACK) ].matchesWon);

  // Make sure we don't divide by zero on the line below.  
  if (playerData[0].totalScore == 0 && playerData[1].totalScore == 0)
  {
    playerData[0].totalScore = 1;
  }
  
  // Calculate and display the average time per score.
  int avgScoreSecs = (int) ( totalGameSeconds / ( playerData[0].totalScore + playerData[1].totalScore ) );
  int avgScoreMins = 0;
  if (avgScoreSecs >= 60)
  {
    avgScoreMins = (int) (avgScoreSecs / 60);
    avgScoreSecs = avgScoreSecs % 60;
  }
  
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 10, avgScoreMins);
  genieWriteObject(GENIE_OBJ_LED_DIGITS, 11, avgScoreSecs);
  
  genieWriteObject(GENIE_OBJ_FORM, 1, 0);
}
