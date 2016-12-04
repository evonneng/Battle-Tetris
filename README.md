# Battle-Tetris
Battle Tetris game programmed for an ARM-Cortex tm4c123 microcontroller requiring hardware components: 2 LCD's, 8 switches, 2 slide pots, and an audio jack. 

by Calvin Ly and Evonne Ng

##Overview
In the game Tetris, you move around a random sequence of falling 4 piece blocks around the screen with the aim of creating a horizontal line of ten units without gaps. You manipulate these Tetriminos by moving each piece sideways or rotating by 90 degrees. When such a line is created, it disappears. As the game progresses, the player loses once the stack of Tetrominoes reaches the top of the screen. 


##Basic Rules
1. There are 7 standard Tetris pieces
2. Move active Tetris piece left, right, and down
3. Can rotate active Tetris piece
4. The active piece moves downwards on a timer
5. Once active piece is placed, a new active piece spawns at the top of the board
6. Lose when the placed blocks hits the ceiling
7. Clear a line when filling in the entire row

##Features
1. 4 buttons corresponding to moves: left, right, rotate, down
2. Classic Tetris music
3. Slide bar controlling the speed in which the pieces drop down
4. 2 player battle Tetris - use the UART to send garbage lines to the other player that spawn at the bottom of the screen, try to follow the rules of tetrisfriends.com (perhaps some simplifying changes) 
5. Sent lines will have a single block missing in them to be able to be cleared easily

##Difficulty
Tetris Rotations (collisions, wall kicks)
UART communications to send game status/lines sent between them
