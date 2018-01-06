# NOTES 

### LINE-FOLLOWING ROBOT

* I could start from the object avoiding robot. 
* Changed the AVOID_APROX variables for better adjustment.
* The Subsumption scheme would be, from most important to less:
	1. Avoid getting stuck in corners.
	2. Avoid obstacles.
	3. Find a wall.
	4. Move in a straight line. 

---

### GARBAGE-COLLECTION ROBOT

* Placing cans for the garbage.
* No way to differ when the robot is stopped and when is moving, so touching the wall with the cans is not an option. Made a black line around the border so I could use ground sensors for that. 



























---