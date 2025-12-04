Assignment-2
objective-1
OVERVIEW
This repository is the fork of the original game provided by the our teacher.This assignment focuses on understanding the existing game code,analyzing its logic,and making improvements using C programming concepts like arrays,loops,pointers,and functions.
ORIGINAL GAME
This game is a lane-avoidance console game.The player controls a character at the bottom of the screen and can move left or right between 3 lanes.Obstacles fall from the top in random lanes,and the player must dodge them.Each avoided obstacle increases the score,and the game gradually speeds up.The player has 3 lives and the game ends when all lives are lost.
STRUCTURE OF CODE
Header Files:Include libraries for input/output ,keyboards control,screen control,randomnumbers,timing and sound.
clear_screen_fast()Function-Moves cursor to the top-left to redraw the game smoothly without flickering.
Main Function Setup
  Sets console color
  Starts background music
  Initailizes player position,obstacle position,score,lives,and speed
Game LOOP(while(1))
   Input Handling-Checks  row key presses to move the player left or right
   Drawing-Prints the game area,obstacle and player in their current lanes
   Collions Detection-Checks if obstacle hits the player reduces lives or increases score
   Speed Adjustment-Increases game difficulty as score increases
   Obstacle Movement-Moves obstacle downward and resets it to a new random lane after reaching the bottom
Game over Condition
   when lives reach zero,impact sound plays and game stops.
MODIFICATIONS
Score system-integer counter
Difficulty level -increases as speed over time
high score storage-counts to more value




   
