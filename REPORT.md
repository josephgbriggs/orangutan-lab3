Deliverables
------------
Hand in all of the code necessary to compile your project. Please put sufficient comments in your code so that I can follow what you are doing. Also, if your code is not working in some aspect, please include that in your report. In addition, submit a report that addresses the following:

1. Implement the controller to maintain speed. The pieces to experiment with are the sampling rate and the range of reference speed. Note that you can only achieve about 5 degrees of accuracy due to the sensor, thus you will need to calculate speed using a sliding window of averaged measurements or slow your sampling rate. Attempt to maintain a very high speed (I think 1.5 rotations/second is about maximum) and a very low speed (how slow can you go??) and some values in between. Start by tuning your gains for the very fast. Try those same gains for the very slow. Record the measured position, measured speed, reference speed, and error as the motor is moving. Report the gains and the sampling rate. Graph the variables and describe and explain your findings. Can you use the same gains for the full range of reference speed, or is there a point at which you have to change gains?

	>
	>

2. Implement the controller to maintain position. Using a good sampling rate, experiment with speed and range of reference position. Use high gains for positional control, so that the motor mostly runs at full speed. Observe the overshoot and see if you can adjust the gain to maintain both a high speed and not overshoot. Then use very low gains so that the motor runs very slowly. Observe for undershoot and see if you can adjust the gains to maintain both low speed and reach the target position. For each, record the measured position, measured speed, reference position, nd error; and record your gains. Graph the variables and describe and explain your findings.

	>
	>

3. Determine optimally tuned values for the PD positional controller (i.e. those that achieve good control while maintaining good speed) and the optimal frequency of the controller based on above experiment. Implement the interpolator and execute the trajectory: rotate the motor forward 360 degrees, hold for .5 seconds (have the system precisely measure this time period), then rotate backwards for 360 degrees, hold for .5 seconds, rotate forwards for 10 degrees. Graph Pm, Pr and T while executing the trajectory. Be sure to graph the entire trajectory.

	>
	>

4. Execute the same trajectory described above, except run your PD controller at 50Hz and then at 5Hz while graphing the same variables. Discuss the results.

	>
	>