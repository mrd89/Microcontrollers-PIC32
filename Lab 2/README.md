Written by: Matthew Daniel, Ximeng Zhang, and Gregory Kaiser 

Date: 2019-September


### Goal of Lab

Produce a game in which ball-like particles enter from one side of the screen. The paddle position will be controlled by an analog input potentiometer to catch balls. There will be a time limit to the game. Display will be on an 320x240 TFT LCD, with DMA sound effects. The balls will follow standard billards-type dynamics, with zero friction between balls. The sound must be produced through a DAC output using a DMA channel.



### Requirements

- When you press the game start button (not reset), the program should:
	- draw a playing field consisting of a rectangle 320 wide x 240 high on the LCD screen with two internal barriers.
	The barriers should be placed at about 1/3 the x-width of the screen and and about 1/4 of the screen long.
	Balls bounce off the sides of the barriers.

	- set the running time clock to zero. The clock should read elapsed play time on secreen.
	start firing balls of radius 2 pixels onto the right hand edge of the screen with vx=-1 to -3 pixels/frame and vy=+1.5 to -1.5 pixels/frame.

	- The speed distribution in the y-direction must be uniform and random. The x speed can be fixed between the values indicated.
	You can vary this to make the game more playable, if necessary. The ball icons can be very simple.

	- draw a "paddle" consisting of a line segment with a size about 5 times the size of the balls. You can vary this to make the game more playable, or to produce harder levels of play.

- Set the TFT frame time using a thread to be faster than 15/second.
Since the computation will be the most demanding calculation and depends on the number of balls,
arrange the thread to produce a constant frame rate, while allowing as much time as possible for computation.
(pseudocode example of constant frame rate)
Display the frame rate on the screen in frames/sec.
- At each frame time, update the velocity and position of all the balls on the screen, and redraw the paddle.
The drawing of the paddle need not be complicated.
- Balls which are hit on the front of the paddle increment your score, and the balls are removed from the screen.
- Balls which get past the paddle to the left side of the screen decrement your score, and the balls are removed from the screen.
- A score, a frame rate, number of current balls, and time should be displayed.
- All balls can be deflected by other balls according to the hard ball dynamics given above.
- Paddle vertical position on the screen should be changed by a potientiometer attached to the A/D converter.
The paddle horizontal position will be fixed at around x=20.
- The sides of the barriers, and the walls reflect balls according to: angle incidence equals angle of reflection.
- The game ends after a fixed time, which you can choose.
- New balls should enter the playing field at regular intervals (from the right hand edge), perhaps a few per second.
Balls which leave the screen are immediately recycled to to sent in again.
- Use fixed point to incrase performance
- There should be minimal visual artifacts (tearing, flickering) during operation.
- There are required sound effects, which for full credit, must be generated using DMA-driven, SPI DAC output, with NO sound-generation ISR.
Audio synthesis rate should b 44 Ksamples/sec for at least one sound. Others can be at lower rate.
You need separate sounds for +1 score, -1 score, and for game end.
