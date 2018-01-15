# GARBAGE-COLLECTION ROBOT

## OVERVIEW

The next robot is programmed to clean the arena of garbage. It searches for "garbage items" in the arena (represented with cans), and pushes it by the wall. The robot continues the process until all garbage has been cleaned. 

To program this behaviour, we have started from the controller of the previous robot, modifying the subsumption architecture to fit the new needs. In the end, the subsumption levels were the following:

1. Dispose the garbage, turning around when it is by the wall.
2. Avoid every obstacles.
3. Push the garbage in a straight line, looking for the wall.
4. Look for garbage.

![](images/GarbageDiagram.jpg)

We already explained how subsumption architectures work in the previous robot overview. The most basic level guides the robot in the search for garbage to clean. In case the robot has already found garbage to clean, the upper level subsumes this behaviour, making the robot push the garbage. In this process, we must make sure that the robot does not get stuck with obstacles, so the third level takes control when an obstacle is detected, so the robot can avoid it. The top level just makes the robot turn around when the garbage has already been disposed in the disposal area. 

In this case, the fact that we started from the controller of the previous robot makes the code structure not that evident. The `if-else` statements no longer represent the subsumption architecture. This is also because of the limitations of the robot itself and its sensors. Obstacle detection is carried out using the proximity sensors, but *Webots* documentation estates that the e-puck´s proximity sensors are reliable when obstacles are at least 4cm close. This is a huge bottleneck for a few reasons:

* There is no way to detect when the robot is pushing garbage that is already by the wall. Proximity sensors 0 and 7 are useful to know that the garbage is in front of the robot, but sensors 1 and 6 are not capable of detecting the wall behind. This is because the diameter of the cans that represent the garbage is bigger than 4cm.
* This problem extends to the obstacles in the middle of the arena. The robot might push a can against an obstacle and there would not be a way to notice that situation. 

These problems had to be solved, instead of modifying the controller code, modifying the robot´s environment, so different sensors could be used. We decided to mark the disposal area and obstacles areas with black color in the ground, the rest being white. This would allow to use the ground sensors to detect obstacles and the disposal area, and the proximity sensors to find and push garbage. 

The code, like with the previous robot, has a main sequence of `if-else` statements that "decide" which behaviour should be executed. As said before, in this case this statements do not directly represent the subsumption architecture.

* The first statement is not a part of the architecture, but a way to continue the execution of previous behaviours. When turning around, we used the static variables `turning_around_left` and `turning_around_right` as counters, so the robot could keep on turning the next few iterations of the main loop.
* The next statement checks if the robot has touched a wall, what could be the real wall or an obstacles. Takes a look at the values of the ground sensors and, in case these detect a black area, then the robot starts turning away.
* The next statement runs if the robot is pushing garbage. Uses the proximity sensors 0 and 7 to push it in a straight line, working in a similar way to the robot that followed the black line: moving slightly right or slightly left depending on which of them detects the higher value. 
* The last statement runs only if all the rest did not, which means that there are no obstacles or walls, and the robot is not turning or pushing garbage. In that case, the method `look_for_garbage()` is called. It makes the robot move in a straight line, deviating only if a proximity sensor measures a value bigger than a threshold (80). This does not work perfectly due to the previously mentioned limitations of the proximity sensors, but it has helped find cans faster in some cases. 

As we can see, the "AVOID OBSTACLES" level and the "DISPOSE GARBAGE" level are united in a single statement. It works because the environment demands little differentiation between the disposal area and the obstacles. Both situations, being in the disposal area or in front of an obstacle, require the robot to turn around. That could mean leaving a can in the disposal area, which is considered a success, or in front of an obstacle. The latter could seem like a big problem, but in the testing phase we will see that does not stop the robot from cleaning that can in the end. 
















## TESTING