Deliverables
------------
Hand in all of the code necessary to compile your project. Please put sufficient comments in your code so that I can follow what you are doing. Also, if your code is not working in some aspect, please include that in your report. In addition, submit a report that addresses the following:

1. Implement the controller to maintain speed. The pieces to experiment with are the sampling rate and the range of reference speed. Note that you can only achieve about 5 degrees of accuracy due to the sensor, thus you will need to calculate speed using a sliding window of averaged measurements or slow your sampling rate. Attempt to maintain a very high speed (I think 1.5 rotations/second is about maximum) and a very low speed (how slow can you go??) and some values in between. Start by tuning your gains for the very fast. Try those same gains for the very slow. Record the measured position, measured speed, reference speed, and error as the motor is moving. Report the gains and the sampling rate. Graph the variables and describe and explain your findings. Can you use the same gains for the full range of reference speed, or is there a point at which you have to change gains?

	> Not completed. See Project Issues explanation below.
	> 

2. Implement the controller to maintain position. Using a good sampling rate, experiment with speed and range of reference position. Use high gains for positional control, so that the motor mostly runs at full speed. Observe the overshoot and see if you can adjust the gain to maintain both a high speed and not overshoot. Then use very low gains so that the motor runs very slowly. Observe for undershoot and see if you can adjust the gains to maintain both low speed and reach the target position. For each, record the measured position, measured speed, reference position, nd error; and record your gains. Graph the variables and describe and explain your findings.

	> Not completed. See Project Issues explanation below.
	> 

3. Determine optimally tuned values for the PD positional controller (i.e. those that achieve good control while maintaining good speed) and the optimal frequency of the controller based on above experiment. Implement the interpolator and execute the trajectory: rotate the motor forward 360 degrees, hold for .5 seconds (have the system precisely measure this time period), then rotate backwards for 360 degrees, hold for .5 seconds, rotate forwards for 10 degrees. Graph Pm, Pr and T while executing the trajectory. Be sure to graph the entire trajectory.

	> Not completed. See Project Issues explanation below.
	> 

4. Execute the same trajectory described above, except run your PD controller at 50Hz and then at 5Hz while graphing the same variables. Discuss the results.

	> Not completed. See Project Issues explanation below.
	> 
	
Project Issues
--------------
Unfortunately I had some hardware issues that interfered with completion of this assignment. The encoder was not behaving reliably enough to get the correct wheel position which made subsequent control of speed and position impossible.

Further below is an except of a log showing some values from motor.c that were supplied by the encoder. This logging occurred in the interrupt service handler that was triggered whenever there was a change on the encoder pins on the wheel watcher. 

Here is a snippet from that log with an explanation of the entries:

	 1  ----- motor.c -----
	 2  µs: 19319567  µs-last: 19319571  µs∆: 8124
	 3  cnt: 405  pos∆: 1
	 4  enA: 1  enB: 2  plus: 1  minus: 3
	 5  edir: 0  pos: 21  err: 0
	 6  mdir: 1  tor: 255  vel: 123

The values captured are (each bullet below is a line in the log): 

* The current elapsed time in microseconds; the time as the last pin change; and the time delta in microseconds
* The count of encoder positions (an odometer that can roll back); and the change in wheel postion since last signal (a sanity check)
* The values on the encoder pins A and B; as well as the calculations for whether the encoder thinks we are going forward (plus==1) or backward (minus==1)
* The direction the coder thinks we are going based on plus and minus (0==forward, 1==backward); the position of the wheel from 0 to 127; and whether we think we have detected an encoder error
* The direction the motor thinks we are going based on torque input (1==forward); the torque applied from -255 to 255; and the velocity of the wheel (in wheel positions per second)

Here is the log:

	 1  ----- motor.c -----
	 2  µs: 19319567  µs-last: 19319571  µs∆: 8124
	 3  cnt: 405  pos∆: 1
	 4  enA: 1  enB: 2  plus: 1  minus: 3
	 5  edir: 0  pos: 21  err: 0
	 6  mdir: 1  tor: 255  vel: 123
	 7  ----- motor.c -----
	 8  µs: 19319512  µs-last: 19319567  µs∆: 6079
	 9  cnt: 406  pos∆: 1
	10  enA: 0  enB: 2  plus: 2  minus: 3
	11  edir: 0  pos: 22  err: 0
	12  mdir: 1  tor: 255  vel: 164
	13  ----- motor.c -->ERROR
	14  µs: 19319522  µs-last: 19319512  µs∆: 6079
	15  cnt: 407  pos∆: N/C
	16  enA: 1  enB: 0  plus: -1  minus: 1
	17  edir: 0  pos: 23  err: 1
	18  mdir: 1  tor: 255  vel: N/C
	19  ----- motor.c -->ERROR
	20  µs: 19319592  µs-last: 19319522  µs∆: 6079
	21  cnt: 408  pos∆: N/C
	22  enA: 0  enB: 2  plus: -1  minus: 1
	23  edir: 0  pos: 24  err: 1
	24  mdir: 1  tor: 255  vel: N/C
	25  ----- motor.c -----
	26  µs: 19319559  µs-last: 19319592  µs∆: 4551
	27  cnt: 409  pos∆: 1
	28  enA: 0  enB: 0  plus: 2  minus: 0
	29  edir: 0  pos: 25  err: 0
	30  mdir: 1  tor: 255  vel: 219
	31  ----- motor.c -----
	32  µs: 19319536  µs-last: 19319559  µs∆: 3407
	33  cnt: 410  pos∆: 1
	34  enA: 1  enB: 0  plus: 1  minus: 0
	35  edir: 0  pos: 26  err: 0
	36  mdir: 1  tor: 255  vel: 293
	37  ----- motor.c -->ERROR
	38  µs: 19319550  µs-last: 19319536  µs∆: 3407
	39  cnt: 411  pos∆: N/C
	40  enA: 0  enB: 2  plus: -1  minus: 1
	41  edir: 0  pos: 27  err: 1
	42  mdir: 1  tor: 255  vel: N/C
	43  ----- motor.c -----
	44  µs: 19319584  µs-last: 19319550  µs∆: 2563
	45  cnt: 412  pos∆: 1
	46  enA: 0  enB: 0  plus: 2  minus: 0
	47  edir: 0  pos: 28  err: 0
	48  mdir: 1  tor: 255  vel: 390
	49  ----- motor.c -->ERROR
	50  µs: 19319594  µs-last: 19319584  µs∆: 2563
	51  cnt: 413  pos∆: N/C
	52  enA: 1  enB: 2  plus: -1  minus: 1
	53  edir: 0  pos: 29  err: 1
	54  mdir: 1  tor: 255  vel: N/C

As you see in line 4 above, the encoder values already look a little wonky with encoder B taking on the value of '2', but that is something that could probably be coded around by looking at non-zero values instead of relying on the pololu function to return a '1' if there is a high value present. What can't be coded around is the erratic pattern of the encoder values.	

Below are the encoder lines from the first three entries in the log above, snipped out for easier comparison.

	 4  enA: 1  enB: 2  plus: 1  minus: 3
	...
	10  enA: 0  enB: 2  plus: 2  minus: 3
	...
	16  enA: 1  enB: 0  plus: -1  minus: 1

As you can see, between lines 4 and 10, only one of the encoder values changed. Encoder A changed while encoder B stayed the same. This is because the encoders are offset by 90˚ and only one should change at a time. However, if you look at lines 10 and 16, both the encoder A and encoder B values changed. This leads to an error in trying to determine the position of the wheel and any other calculations that could be made.