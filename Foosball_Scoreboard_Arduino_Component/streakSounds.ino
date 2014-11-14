// Plays the sound for a given scoring streak.
void playStreakSound(int streak)
{
  int soundToPlay = soundMap.score;
  
  if (streak == 2)
  {
    soundToPlay = soundMap.ks_2_double_kill;
  }
  else if (streak == 3)
  {
    soundToPlay = soundMap.ks_3_triple_kill;
  }
  else if (streak == 4)
  {
    soundToPlay = soundMap.ks_4_overkill;
  }  
  else if (streak == 5)
  {
    soundToPlay = soundMap.ks_5_killtacular;
  }
  else if (streak == 6)
  {
    soundToPlay = soundMap.ks_6_killing_spree;
  }
  else if (streak == 7)
  {
    soundToPlay = soundMap.ks_7_killing_frenzy;
  }
  else if (streak == 8)
  {
    soundToPlay = soundMap.ks_8_running_riot;
  }
  else if (streak == 9)
  {
    soundToPlay = soundMap.ks_9_rampage;
  }
  else if (streak == 10)
  {
    soundToPlay = soundMap.ks_10_untoucable;
  }
  else if (streak == 11)
  {
    soundToPlay = soundMap.ks_11_invincible;
  }
  else if (streak == 12)
  {
    soundToPlay = soundMap.ks_12_inconceivable;
  }
  else if (streak == 13)
  {
    soundToPlay = soundMap.ks_13_un_friggin_believable;
  }
  else if (streak == 14)
  {
    soundToPlay = soundMap.ks_14_extermination;
  }
  else if (streak == 15)
  {
    soundToPlay = soundMap.ks_15_killjoy;
  }
  
  genieWriteObject(GENIE_OBJ_SOUND, 0, soundToPlay);
}
