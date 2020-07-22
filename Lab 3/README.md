Written by: Matthew Daniel, Ximeng Zhang, and Gregory Kaiser 

Date: 2019-October


### Goal of Lab
Build a one degree-of-freedom hovering device which allows the user to select the hover angle then controls the lift-motor with user-selectable parameters to maintain constant angle as the load changes. The motor output signal and actual angle will be displayed as waveforms on the oscilloscope. You will use ProtoThreads and write your program as tasks, but the control loop will need to run in an ISR.


### Requirements
- Measure the angle of the beam supporting the lift-motor.
- Format the set angle and PID parameters to display appropriate messages on an LCD.
- At any time, take commands from the PC keyboard to:
	- Set the desired beam angle.
	- Set the PID proportional gain.
	- Set the PID differential gain.
	- Set the PID integral gain.
	- To enter values on the PC keyboard, you will use command formats like:
		- a 300
		- p 10
		- i 2
		- d 5.2
	- The new value takes effect immediately.
	- One set of coefficients should produce stable behavior over the range of desired hover angles.
- Use a PID algorithm to control the speed of the motor by producing a PWM drive to the optoisolator. PWM setup example.
- Tune the PID algorithm so that you can change the angle of the beam quickly and accurately without excessive angle oscillations.
- The user should be able to enter a desired hover angle and the motor should quickly change the beam to the new angle.
The initial angle of the beam should be around -1.57 radian (hanging straight down).
- Display the motor control value (not the raw PWM signal) on the scope using channel B of the DAC.
Noise control on this signal is essential. 
- Display the actual beam angle, from the angle sensor, on the scope using channel A of the DAC.
If this signal is noisy, you need to find out why! Poor wire routing, loose connections, and motor noise are
all possibilities. A noisy input makes the control PID algorithm very hard to tune.
- When a button is pushed (not the reset button), the beam should go through a
quick sequence of defined angle changes, using the current set of PID parameters:
	- Before time=0, while holding the button, the beam should be hanging vertically down (motor off).
	- When the button is released at time=0, target angle should be set to horizontal.
	- At time=5 seconds, target angle should be set to approximately 30 degrees above horizontal.
	- At time=10 seconds, target angle should be set to approximately 30 degrees below horizontal.
	- At time=15 seconds, target angle should be set to horizontal.

