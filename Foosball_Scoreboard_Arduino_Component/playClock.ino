// Clock variables internal to this file.
volatile int clockSeconds;
volatile int clockMinutes;
int displayedMinutes = 0;                    // Keep track of current minute is to optimize serial calls.
int pulseCount;

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
  // Only fire off the serial call to update the minute display if it's actually changing.
  if (displayedMinutes != clockMinutes)
  {
    displayedMinutes = clockMinutes;
    genieWriteObject(GENIE_OBJ_LED_DIGITS, 4, displayedMinutes);
  }
  
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

