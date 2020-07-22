Written by: Matthew Daniel & Ximeng Zhang
Date: 2019-September


Goal of this lab: produce a music synthesizer, played using a keypad, and with a programmable playback function.

###Overall Requirements:

- The program starts In play mode. The program will play the notes C4 through C5 using the keypad keys 1 to 8.

- Each note will play for the whole duration of attack-sustain-decay, exactly once, independent of the duration of the button push.

- The notes will play with an amplitude envelope and spectrum that you choose, but cannot be a boring pure sine wave, or a harsh square wave.
Perhaps use FM synthesis to make a sound which is string-like or drum-like.

- The program will produce a rest (no sound) when you press keypad key 0.

- This is mostly usful in record mode (see below).

- In play mode the program will play the recorded (see below) note sequence at a fixed rate, perhaps one or two notes/second when the # key is pressed.

- The notes will play with an envelope and spectrum that you choose, but cannot be just a pure sine wave.

-The program should have three modes:

	1. Upon power-up the system should go to play mode and play a note for every keypad press.
	This mode requires keypad debouncing.

	2. Pressing and holding a separate button (not the keypad) puts the system into frequency test mode for frequency calibration.
	In this mode each button, 1 to 8, produces a pure sine wave for as long as it is pushed, at full intensity, with no envelope.

	3. Pressing and holding another button (not the keypad) puts the system into record mode so that each keypad press is recorded for later playback. Recording continues 		until the record mode button is released. This mode requires keypad debouncing. The duration of each keypad key press does not affect the recording.

- The program should support one timbre (envelope and spectral content) that you can hard-code.
- There should be no clicks, pops, or other audio artifacts of synthesis.
