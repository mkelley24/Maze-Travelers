# Maze-Travelers
## Summary
This project is a a simulation of several travelers  randomly navigating a maze together. These travelers must not run through the orange walls, the tails of travelers, or the sliding gray partitions. If they run into a sliding partition they can move it to get past it. Each traveler is run on its own thread. Due to each traveler being its own thread race conditions became a possible issue, since they were sharing the resource of the grid. To prevent these issues I created a matrix of mutex locks to correspond with each square in the maze. when the traveler is mocing they will lock the squares that they would need to make there move, ie the squares in their body, aswell as the potential next move squares, and when sliding partitions are being moved, they make sure to lock the squares that they could be moved into. The use of mutex locks prevent the issue of a race condition from occuring, but opens up the potential of deadlocks if the locks are not handled properly. Whenever a traveler needed to lock squares on the grid, it would fill a set with the coordinates of the squares it wanted to mutex lock. Then the functions, `lockTheLockSet()` and `unlockTheLockSet()` would take the set and make sure that the locks were locked/unlocked inorder of left to right, top to bottom, since always grabbing locks from "lowest" to "highest" is a way to prevent the cyclical waiting of a deadlock.

## Background
This project was the Final Project for Operating Systems and Networks (CSC 412) at URI taught by Professor Herve in the Fall 2023 semester. The professor gave us some base code to work off of, ie the user interface, and the maze generator, while I coded the movement and collision detection of the travelers, the thread creation of the travelers, the movement of the partitions, and the mutex lock management. The bulk of my code is found between line 543 and line 1390 on main.cpp, in addition to my function prototypes, global variables, and global variable initialization found near the top of the code and in the main function respectively.

## Running
This code will run on linux and MacOS but not on windows, there will be a video below of the code running so you can see it in action if you are on windows, or if you don't want to install the code.
### Linux instructions
1. First you must have GLUT installed which can be installed by running these two commands:
   * `sudo apt-get install freeglut3-dev`
   * `sudo apt-get install mesa-common-dev`
2.  Next you compile the code by running the `build.sh` script, which will create a executable called `Maze`
3.  Finally you run the code by typing the command `./Maze <mazeWidth> <mazeHeight> <travelerCount> <travelerGrowthRate>?` (the traveler growth rate is how many moves the traveler will move before growing one additional segment to their tail, it is an optional argument, if left off the traveler will never grow an additional segment)

### MacOS instructions
1. Unlike linux MacOS has glut by default so you do not need to download anything.
2. Next, you compile the code by running the `mac_build.sh` script.
3.  Finally you run the code by typing the command `./Maze <mazeWidth> <mazeHeight> <travelerCount> <travelerGrowthRate>?` (the traveler growth rate is how many moves the traveler will move before growing one additional segment to their tail, it is an optional argument, if left off the traveler will never grow an additional segment)

## Example

https://github.com/mkelley24/Maze-Travelers/assets/78382692/1dffb8c0-ffd4-4e15-a284-88c4f2b3681b

