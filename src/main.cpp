//
//  main.c
//  Final Project CSC412
//
//  Created by Jean-Yves Hervé on 2020-12-01, rev. 2023-12-04
//
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.

#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <mutex>
#include <set>
#include <utility>
//
#include <cstdio>
#include <cstdlib>
#include <ctime>
//
#include <unistd.h>
//
#include "gl_frontEnd.h"

//	feel free to "un-use" std if this is against your beliefs.
using namespace std;

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Private Functions' Prototypes
//-----------------------------------------------------------------------------
#endif

void initializeApplication(void);
GridPosition getNewFreePosition(void);
Direction newDirection(Direction forbiddenDir = Direction::NUM_DIRECTIONS);
TravelerSegment newTravelerSegment(const TravelerSegment& currentSeg, bool& canAdd);
void generateWalls(void);
void generatePartitions(void);

// my function prototypes Code: DELTA
//threadfunc that controls the travelers
void runnerThread(int travelerIndex);

//returns a TravSeg struct corresponding to a move in the given direction
TravelerSegment directionalMove(TravelerSegment &head, Direction pathDir);

//returns true if the move is inbounds (not critical)
bool isValid(TravelerSegment nextMove);

//returns true if the move is into a free or exit square (critical)
bool isFree(TravelerSegment nextMove);

//returns true if the traveler segement is an exit square (not critical)
bool isExit(TravelerSegment nextMove);

//runs through the moves in the 4 directions and adds the legal moves to the move list (critical)
void loadMoveList(vector<TravelerSegment> &nextMoveList, TravelerSegment &head);

//returns the inverse of the given direction ie North's inverse is South and West's inverse is east (not critical)
Direction inverseDirection(Direction trajectory);

//is given a move direction for the traveler and handles the travelers movement and tail growth (critical)
void moveTraveler(Traveler &runner, TravelerSegment newHead, int &moveCounter);

//deprecated
bool changeDirection(Traveler &runner, Direction trajectory);

//randomly selects a move from our list of legal moves (critical because it relies on a check from critical)
TravelerSegment selectMove(vector<TravelerSegment> &nextMoveList);

//removes a traveler from the grid upon completetion of the maze (critical)
void eraseTraveler(Traveler &runner);

//returns true if a squaretype is either type of partition (not critical)
bool isPartition(TravelerSegment nextMove);

//returns true if the traveler is moving into the broadside of a partition ie a traveler moving west into a vertical partition
bool congruentPartition(TravelerSegment nextMove);

//partition handling(all critical):

	//vertical partition handling:
//returns true if the traveler can pass by moving the partition up
bool canMoveUp(TravelerSegment nextMove);

//returns true if the traveler can pass by moving the partition up
bool canMoveDown(TravelerSegment nextMove);

//returns true if the traveler can pass by moving the partition
bool canMoveVerticaly(TravelerSegment nextMove);

//returns the gridPos of the top of a vertical partition
GridPosition findPartitionTop(TravelerSegment nextMove);

//returns the gridPos of the bottom of a vertical partition
GridPosition findPartitionBottom(TravelerSegment nextMove);

//finds the last consecutive free space above a vertical partition
GridPosition findPartitionUpperEdge(TravelerSegment nextMove);

//finds the last consecutive free space below a vertical partition
GridPosition findPartitionLowerEdge(TravelerSegment nextMove);

//calculates the size of freespace above the partition
int findUpperMoveBuffer(TravelerSegment nextMove);

//calculates the size of freespace above the partition
int findLowerMoveBuffer(TravelerSegment nextMove);

// moves a vertical partition upwards to allow a traveler to pass
void movePartitionUp(TravelerSegment nextMove);

// moves a vertical partition downwards to allow a traveler to pass
void movePartitionDown(TravelerSegment nextMove);

//moves the partition up or down (if both are possible a direction is randomly selected)
void moveVerticalPartition(TravelerSegment nextMove);

	// horizontal partition handling:
//checks if the partition can be moved left
bool canMoveLeft(TravelerSegment nextMove);

//checks if the partition can be moved right
bool canMoveRight(TravelerSegment nextMove);

//checks if the partition can be moved in either direction
bool canMoveHorizontaly(TravelerSegment nextMove);

//finds the left most point of the partition
GridPosition findPartitionLeft(TravelerSegment nextMove);

//finds the right most point of the partition
GridPosition findPartitionRight(TravelerSegment nextMove);

//finds the point on the left side where the partion can be moved to
GridPosition findPartitionLeftEdge(TravelerSegment nextMove);

//finds the point on the right side where the partion can be moved to
GridPosition findPartitionRightEdge(TravelerSegment nextMove);

// returns how much space on the left for the partition to move into
int findLeftMoveBuffer(TravelerSegment nextMove);

//returns how much space on the right for the partition to move into
int findRightMoveBuffer(TravelerSegment nextMove);

//moves the partition to the left
void movePartitionLeft(TravelerSegment nextMove);

//moves the partition to the right
void movePartitionRight(TravelerSegment nextMove);

//moves the partition out of the way (if both directions are valid, one is randomly selected)
void moveHorizontalPartition(TravelerSegment nextMove);

//Lock Section:
//fills the lock grid with all the mutexes for each square
void initializeLockGrid();

//queues locks into a set to keep track of them
void scheduleTravelerLocks(set<pair<int, int> > &lockSet, Traveler runner);

//locks the points in the set
void lockTheLockSet(set<pair<int, int> > &lockSet);

//unlocks the points in the set
void unlockTheLockSet(set<pair<int, int> > &lockSet);

//locks a lock at the points in the pair
void lockPair(pair<int, int> lockPoint);

//unlocks a lock at the points in the pair
void unlockPair(pair<int , int> lockPoint);

//sets all values in verticalpartition map to false
void initializeVerticalPartitionMap();

//writes the vertical partition zones to the map
void writeVerticalPartitionMap();

//writes the upper part of a partition occurence
void writeMapUp(int row, int col);

//writes the lower part of a partition occurence
void writeMapDown(int row, int col);

//sets all the values in horizontal map to false
void initializeHorizontalPartitionMap();

//writes the partition zones to maps
void writeHorizontalPartitionMap();

//writes the left part of partition occurence
void writeMapLeft(int row, int col);

// writes the right part of partition occurence
void writeMapRight(int row, int col);

//checks valid square for partition zone
bool validPartitionZone(int row, int col);



#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Application-level Global Variables
//-----------------------------------------------------------------------------
#endif

// DELTA
//	Don't rename any of these variables
//-------------------------------------
//	The state grid and its dimensions (arguments to the program)
SquareType** grid;
unsigned int numRows = 0;	//	height of the grid
unsigned int numCols = 0;	//	width
unsigned int numTravelers = 0;	//	initial number
unsigned int numTravelersDone = 0;
unsigned int numLiveThreads = 0;		//	the number of live traveler threads
vector<Traveler> travelerList;
vector<SlidingPartition> partitionList;
GridPosition	exitPos;	//	location of the exit

//	travelers' sleep time between moves (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int travelerSleepTime = 60000;

//	An array of C-string where you can store things you want displayed
//	in the state pane to display (for debugging purposes?)
//	Dont change the dimensions as this may break the front end
const int MAX_NUM_MESSAGES = 8;
const int MAX_LENGTH_MESSAGE = 32;
char** message;
time_t launchTime;

//	Random generators:  For uniform distributions
const unsigned int MAX_NUM_INITIAL_SEGMENTS = 6;
random_device randDev;
default_random_engine engine(randDev());
uniform_int_distribution<unsigned int> unsignedNumberGenerator(0, numeric_limits<unsigned int>::max());
uniform_int_distribution<unsigned int> segmentNumberGenerator(0, MAX_NUM_INITIAL_SEGMENTS);
uniform_int_distribution<unsigned int> segmentDirectionGenerator(0, static_cast<unsigned int>(Direction::NUM_DIRECTIONS)-1);
uniform_int_distribution<unsigned int> headsOrTails(0, 1);
uniform_int_distribution<unsigned int> rowGenerator;
uniform_int_distribution<unsigned int> colGenerator;

// my globals code
/**
 * @brief a list containing our taveler threads
 * 
 */
vector<thread> threadList;
int movesToGrow = __INT_MAX__;
bool keepGoing = true;
mutex gridLock;
deque<mutex> travelerLocks;

//lockGrid:
vector<vector<mutex> > lockGrid;

//partition maps
//they keep track of potential partition zones

vector<vector<bool> > verticalPartitionMap;

vector<vector<bool> > horizontalPartitionMap;


#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Functions called by the front end
//-----------------------------------------------------------------------------
#endif
//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================

void drawTravelers(void)
{
	//-----------------------------
	//	You may have to sychronize things here
	//-----------------------------
	for (unsigned int k=0; k<travelerList.size(); k++)
	{
		//	here I would test if the traveler thread is still live
		travelerLocks[k].lock();
		drawTraveler(travelerList[k]);
		travelerLocks[k].unlock();
	}
}

void updateMessages(void)
{
	//	Here I hard-code a few messages that I want to see displayed
	//	in my state pane.  The number of live robot threads will
	//	always get displayed.  No need to pass a message about it.
	unsigned int numMessages = 4;
	sprintf(message[0], "We created %d travelers", numTravelers);
	sprintf(message[1], "%d travelers solved the maze", numTravelersDone);
	sprintf(message[2], "%d travelers remaining in the maze", numTravelers - numTravelersDone);
	sprintf(message[3], "Simulation run time: %ld s", time(NULL)-launchTime);
	
	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//---------------------------------------------------------
	drawMessages(numMessages, message);
}

void handleKeyboardEvent(unsigned char c, int x, int y)
{
	int ok = 0;

	switch (c)
	{
		//	'esc' to quit
		case 27:
			//set keep going to false
			keepGoing = false;
			//thread join
			for(int i = 0; i < (int)threadList.size(); i++){
				threadList[i].join();
				numLiveThreads--;
			}
			exit(0);
			break;

		//	slowdown
		case ',':
			slowdownTravelers();
			ok = 1;
			break;

		//	speedup
		case '.':
			speedupTravelers();
			ok = 1;
			break;

		default:
			ok = 1;
			break;
	}
	if (!ok)
	{
		//	do something?
	}
}


//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupTravelers(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * travelerSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		travelerSleepTime = newSleepTime;
	}
}

void slowdownTravelers(void)
{
	//	increase sleep time by 20%.  No upper limit on sleep time.
	//	We can slow everything down to admistrative pace if we want.
	travelerSleepTime = (12 * travelerSleepTime) / 10;
}




//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	initialization of the various global variables and lists
//------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	//	We know that the arguments  of the program  are going
	//	grid, the number of travelers, etc.
	//	So far, I hard code-some values

	numCols = atoi(argv[1]); // width
	numRows = atoi(argv[2]); // height
	
	numTravelers = atoi(argv[3]);
	if(argc == 5){
		movesToGrow = atoi(argv[4]);
	}
	// vector<mutex> tempLocks(numTravelers);
	travelerLocks.resize(numTravelers);
	// for(int i = 0; i < (int)numTravelers; i++){
	// 	tempLocks.push_back(mutex);
	// }
	numLiveThreads = 0;
	numTravelersDone = 0;
	initializeLockGrid();

	//	Even though we extracted the relevant information from the argument
	//	list, I still need to pass argc and argv to the front-end init
	//	function because that function passes them to glutInit, the required call
	//	to the initialization of the glut library.
	initializeFrontEnd(argc, argv);
	
	//	Now we can do application-level initialization
	initializeApplication();

	launchTime = time(NULL);
	

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	//	Free allocated resource before leaving (not absolutely needed, but
	//	just nicer.  Also, if you crash there, you know something is wrong
	//	in your code.
	for (unsigned int i=0; i< numRows; i++)
		delete []grid[i];
	delete []grid;
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		delete []message[k];
	delete []message;
	
	//	This will probably never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}


//==================================================================================
//
//	This is a function that you have to edit and add to.
//
//==================================================================================


void initializeApplication(void)
{
	//	Initialize some random generators
	rowGenerator = uniform_int_distribution<unsigned int>(0, numRows-1);
	colGenerator = uniform_int_distribution<unsigned int>(0, numCols-1);

	//	Allocate the grid
	grid = new SquareType*[numRows];
	for (unsigned int i=0; i<numRows; i++)
	{
		grid[i] = new SquareType[numCols];
		for (unsigned int j=0; j< numCols; j++)
			grid[i][j] = SquareType::FREE_SQUARE;
		
	}

	message = new char*[MAX_NUM_MESSAGES];
	for (unsigned int k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = new char[MAX_LENGTH_MESSAGE+1];
		
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	//	Yes, I am using the C random generator after ranting in class that the C random
	//	generator was junk.  Here I am not using it to produce "serious" data (as in a
	//	real simulation), only wall/partition location and some color
	srand((unsigned int) time(NULL));

	//	generate a random exit
	exitPos = getNewFreePosition();
	grid[exitPos.row][exitPos.col] = SquareType::EXIT;

	//	Generate walls and partitions
	generateWalls();
	generatePartitions();
	writeHorizontalPartitionMap();
	writeVerticalPartitionMap();
	
	//	Initialize traveler info structs
	//	You will probably need to replace/complete this as you add thread-related data
	float** travelerColor = createTravelerColors(numTravelers);
	for (unsigned int k=0; k<numTravelers; k++) {
		GridPosition pos = getNewFreePosition();
		//	Note that treating an enum as a sort of integer is increasingly
		//	frowned upon, as C++ versions progress
		Direction dir = static_cast<Direction>(segmentDirectionGenerator(engine));

		TravelerSegment seg = {pos.row, pos.col, dir};
		Traveler traveler;
		traveler.segmentList.push_back(seg);
		grid[pos.row][pos.col] = SquareType::TRAVELER;

        //    I add 0-n segments to my travelers
        unsigned int numAddSegments = segmentNumberGenerator(engine);
        TravelerSegment currSeg = traveler.segmentList[0];
        bool canAddSegment = true;
        cout << "Traveler " << k << " at (row=" << pos.row << ", col=" <<
        pos.col << "), direction: " << dirStr(dir) << ", with up to " << numAddSegments << " additional segments" << endl;
        cout << "\t";

        for (unsigned int s=0; s<numAddSegments && canAddSegment; s++){
            TravelerSegment newSeg = newTravelerSegment(currSeg, canAddSegment);
            if (canAddSegment){
                traveler.segmentList.push_back(newSeg);
                grid[newSeg.row][newSeg.col] = SquareType::TRAVELER;
                currSeg = newSeg;
                cout << dirStr(newSeg.dir) << "  ";
            }
        }
        cout << endl;

		for (unsigned int c=0; c<4; c++)
			traveler.rgba[c] = travelerColor[k][c];
		
		travelerList.push_back(traveler);
	}

	
	for(int i = 0; i < (int)travelerList.size(); i++){
		threadList.push_back(thread(runnerThread, i));
		numLiveThreads++;
	}
	
	//	free array of colors
	for (unsigned int k=0; k<numTravelers; k++)
		delete []travelerColor[k];
	delete []travelerColor;
}
//function implementation code: DELTA

void runnerThread(int travelerIndex){
	/*
		need to add a list of traveler locks	
	*/
	//makes the threads wait a second before begining to make viewing it easier
	// sleep(1);
	// cout << "thread begins\n";
	// cout << "starting at " << travelerList[travelerIndex].segmentList[0].row << ", " << travelerList[travelerIndex].segmentList[0].col << "\n";
	vector<TravelerSegment> nextMoveList;
	set<pair<int, int> > lockSet;
	int moveCounter = 0;
	bool stillSearching = true;
	travelerLocks[travelerIndex].lock();
	if(isExit(travelerList[travelerIndex].segmentList[0])){
		stillSearching = false;
	}
	travelerLocks[travelerIndex].unlock();
	while(keepGoing && stillSearching){
		//slows down the travelers so they can be watched
		usleep(travelerSleepTime);
		// gridLock.lock();
		travelerLocks[travelerIndex].lock();
		scheduleTravelerLocks(lockSet, travelerList[travelerIndex]);
		lockTheLockSet(lockSet);
		loadMoveList(nextMoveList, travelerList[travelerIndex].segmentList[0]);
		if(nextMoveList.empty()){
			//gridLock.unlock();
			unlockTheLockSet(lockSet);
			travelerLocks[travelerIndex].unlock();
			// return;
			while (nextMoveList.empty())
			{
				usleep(travelerSleepTime);
				travelerLocks[travelerIndex].lock();
				scheduleTravelerLocks(lockSet, travelerList[travelerIndex]);
				lockTheLockSet(lockSet);
				loadMoveList(nextMoveList, travelerList[travelerIndex].segmentList[0]);
				unlockTheLockSet(lockSet);
				travelerLocks[travelerIndex].unlock();
			}
			
		}
		TravelerSegment nextMove = selectMove(nextMoveList);
		if(congruentPartition(nextMove)){
			// cout << "sq_type: "; // << grid[nextMove.row][nextMove.col] << "\n";
			// cout << (int)grid[nextMove.row][nextMove.col];
			// cout << "\n";
			if(grid[nextMove.row][nextMove.col] == SquareType::HORIZONTAL_PARTITION){
				moveHorizontalPartition(nextMove);
			}
			else if(grid[nextMove.row][nextMove.col] == SquareType::VERTICAL_PARTITION){
				moveVerticalPartition(nextMove);
			}
			else{
				cout << "error in thread partition mover\n";
			}
		}
		moveTraveler(travelerList[travelerIndex], nextMove, moveCounter);
		// gridLock.unlock();
		unlockTheLockSet(lockSet);
		if(isExit(travelerList[travelerIndex].segmentList[0])){
			//cout << "found exit\n";
			stillSearching = false;
		}
		travelerLocks[travelerIndex].unlock();
	}
	travelerLocks[travelerIndex].lock();
	eraseTraveler(travelerList[travelerIndex]);
	travelerLocks[travelerIndex].unlock();
}

TravelerSegment directionalMove(TravelerSegment &head, Direction pathDir){
	TravelerSegment nextMove;
	nextMove.dir = pathDir;
	switch (pathDir)
	{
	case Direction::NORTH:
		nextMove.col = head.col;
		nextMove.row = head.row - 1;
		break;
	case Direction::WEST:
		nextMove.col = head.col - 1;
		nextMove.row = head.row;
		break;
	case Direction::SOUTH:
		nextMove.col = head.col;
		nextMove.row = head.row + 1;
		break;
	case Direction::EAST:
		nextMove.col = head.col + 1;
		nextMove.row = head.row;
		break;
	
	default:
		break;
	}
	return nextMove;
}

bool isValid(TravelerSegment nextMove){
	if(nextMove.row >= numRows || nextMove.row < 0){
		return false;
	}
	else if(nextMove.col >= numCols || nextMove.col < 0){
		return false;
	}
	else{
		return true;
	}
}

bool isExit(TravelerSegment nextMove){
	if(grid[nextMove.row][nextMove.col] == SquareType::EXIT){
		return true;
	}
	else{
		return false;
	}
}

bool isFree(TravelerSegment nextMove){
	if(grid[nextMove.row][nextMove.col] == SquareType::FREE_SQUARE){
		return true;
	}
	else if(grid[nextMove.row][nextMove.col] == SquareType::EXIT){
		return true;
	}
	else{
		return false;
	}
	
}

//partition handling

bool isPartition(TravelerSegment nextMove){
	if(grid[nextMove.row][nextMove.col] == SquareType::HORIZONTAL_PARTITION){
		return true;
	}
	else if(grid[nextMove.row][nextMove.col] == SquareType::VERTICAL_PARTITION){
		return true;
	}
	else{
		return false;
	}
}

bool congruentPartition(TravelerSegment nextMove){
	SquareType partitionSection = grid[nextMove.row][nextMove.col];
	if(partitionSection == SquareType::VERTICAL_PARTITION){
		if(nextMove.dir == Direction::EAST || nextMove.dir == Direction::WEST){
			return true;
		}
		else{
			return false;
		}
	}
	else if(partitionSection == SquareType::HORIZONTAL_PARTITION){
		if(nextMove.dir == Direction::NORTH || nextMove.dir == Direction::SOUTH){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool canMoveUp(TravelerSegment nextMove){
	GridPosition bottomEnd = findPartitionBottom(nextMove);
	// GridPosition topEnd = findPartitionTop(nextMove);
	// GridPosition upperEdge = findPartitionUpperEdge(nextMove);
	int moveLength = ((int)bottomEnd.row - (int)nextMove.row) + 1;
	int moveSpace = findUpperMoveBuffer(nextMove);
	//int moveSpace = (topEnd.row - upperEdge.row);
	if(moveSpace > moveLength){
		return true;
	}
	else{
		return false;
	}
}

bool canMoveDown(TravelerSegment nextMove){
	GridPosition topEnd = findPartitionTop(nextMove);
	// GridPosition bottomEnd = findPartitionBottom(nextMove);
	// GridPosition lowerEdge = findPartitionLowerEdge(nextMove);
	int moveLength = ((int)nextMove.row - (int)topEnd.row) + 1;
	int moveSpace = findLowerMoveBuffer(nextMove);
	//int moveSpace = (lowerEdge.row - bottomEnd.row);
	if(moveSpace > moveLength){
		return true;
	}
	else{
		return false;
	}
}

bool canMoveVerticaly(TravelerSegment nextMove){
	return canMoveUp(nextMove) || canMoveDown(nextMove);
}

GridPosition findPartitionTop(TravelerSegment nextMove){
	int i = 0;
	struct GridPosition partTop;
	partTop.col = nextMove.col;
	while((int)nextMove.row - (i + 1) >= 0){
		if(grid[nextMove.row - (i + 1)][nextMove.col] != SquareType::VERTICAL_PARTITION){
			partTop.row = ((int)nextMove.row - i);
			return partTop;
		}
		i++;
	}
	partTop.row = ((int)nextMove.row - i);
	return partTop;
}

GridPosition findPartitionBottom(TravelerSegment nextMove){
	int i = 0;
	struct GridPosition partBottom;
	partBottom.col = nextMove.col;
	while((int)nextMove.row + (i + 1) < (int)numRows){
		if(grid[nextMove.row + (i + 1)][nextMove.col] != SquareType::VERTICAL_PARTITION){
			partBottom.row = (nextMove.row + i);
			return partBottom;
		}
		i++;
	}
	partBottom.row = (nextMove.row + i);
	return partBottom;
}

// DELTA

GridPosition findPartitionUpperEdge(TravelerSegment nextMove){
	struct GridPosition partTop = findPartitionTop(nextMove);
	struct GridPosition topEdge;
	topEdge.col = partTop.col;
	int i = 0;
	while((int)partTop.row - (i + 1) >= 0){
		if(grid[partTop.row - (i + 1)][partTop.col] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	topEdge.row = partTop.row - i;
	return topEdge;
}

int findUpperMoveBuffer(TravelerSegment nextMove){
	struct GridPosition partTop = findPartitionTop(nextMove);
	// struct GridPosition topEdge;
	// topEdge.col = partTop.col;
	int i = 0;
	while((int)partTop.row - (i + 1) >= 0){
		if(grid[partTop.row - (i + 1)][partTop.col] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	return i;
}

GridPosition findPartitionLowerEdge(TravelerSegment nextMove){
	struct GridPosition partBottom = findPartitionBottom(nextMove);
	struct GridPosition bottomEdge;
	bottomEdge.col = partBottom.col;
	int i = 0;
	while(partBottom.row + (i + 1) < numRows){
		if(grid[partBottom.row + (i + 1)][partBottom.col] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	bottomEdge.row = partBottom.row + i;
	return bottomEdge;
}

int findLowerMoveBuffer(TravelerSegment nextMove){
	struct GridPosition partBottom = findPartitionBottom(nextMove);
	// struct GridPosition bottomEdge;
	// bottomEdge.col = partBottom.col;
	int i = 0;
	while(partBottom.row + (i + 1) < numRows){
		if(grid[partBottom.row + (i + 1)][partBottom.col] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	// bottomEdge.row = partBottom.row - i;
	return i;
}

void movePartitionUp(TravelerSegment nextMove){
	struct GridPosition topEnd = findPartitionTop(nextMove);
	struct GridPosition bottomEnd = findPartitionBottom(nextMove);
	//struct GridPosition upperEdge = findPartitionUpperEdge(nextMove);
	int moveLength = ((int)bottomEnd.row - (int)nextMove.row) + 1;
	//int bufferSize = (topEnd.row - upperEdge.row);
	//int bufferSize = findUpperMoveBuffer(nextMove);
	for(int i = 0; i < moveLength; i++){
		// grid[topEnd.row - (i + 1)][topEnd.col] = SquareType::VERTICAL_PARTITION;
		// grid[bottomEnd.row - i][bottomEnd.col] = SquareType::FREE_SQUARE;
		if(grid[topEnd.row - (i + 1)][topEnd.col] == SquareType::FREE_SQUARE){
			grid[topEnd.row - (i + 1)][topEnd.col] = SquareType::VERTICAL_PARTITION;
		}
		if(grid[bottomEnd.row - i][bottomEnd.col] == SquareType::VERTICAL_PARTITION){
			grid[bottomEnd.row - i][bottomEnd.col] = SquareType::FREE_SQUARE;
		}
	}
}

void movePartitionDown(TravelerSegment nextMove){
	struct GridPosition bottomEnd = findPartitionBottom(nextMove);
	struct GridPosition topEnd = findPartitionTop(nextMove);
	// struct GridPosition lowerEdge = findPartitionLowerEdge(nextMove);
	int moveLength = ((int)nextMove.row - (int)topEnd.row) + 1;
	//int bufferSize = findLowerMoveBuffer(nextMove);
	//int bufferSize = (lowerEdge.row - bottomEnd.row);
	for(int i = 0; i < moveLength; i++){
		if(grid[bottomEnd.row + (i + 1)][bottomEnd.col] == SquareType::FREE_SQUARE){
			grid[bottomEnd.row + (i + 1)][bottomEnd.col] = SquareType::VERTICAL_PARTITION;
		}
		if(grid[topEnd.row + i][topEnd.col] == SquareType::VERTICAL_PARTITION){
			grid[topEnd.row + i][topEnd.col] = SquareType::FREE_SQUARE;
		}
	}
}

void moveVerticalPartition(TravelerSegment nextMove){
	if(canMoveUp(nextMove) && canMoveDown(nextMove)){
		int choice = headsOrTails(engine);
		if(choice == 1){
			movePartitionUp(nextMove);
		}
		else if(choice == 0){
			movePartitionDown(nextMove);
		}
		else{
			cout << "error selecting partition to move\n";
		}
	}
	else if(canMoveUp(nextMove)){
		movePartitionUp(nextMove);
	}
	else if(canMoveDown(nextMove)){
		movePartitionDown(nextMove);
	}
	else{
		std::cout << "can't move partition\n";
	}
}

bool canMoveLeft(TravelerSegment nextMove){
	GridPosition rightEnd = findPartitionRight(nextMove);
	// GridPosition leftEnd = findPartitionLeft(nextMove);
	// GridPosition leftEdge = findPartitionLeftEdge(nextMove);
	int moveLength = ((int)rightEnd.col - (int)nextMove.col) + 1;
	int moveSpace = findLeftMoveBuffer(nextMove);
	//int moveSpace = (leftEnd.col - leftEdge.col);
	if(moveSpace >= moveLength){
		return true;
	}
	else{
		return false;
	}
}

bool canMoveRight(TravelerSegment nextMove){
	GridPosition leftEnd = findPartitionLeft(nextMove);
	//GridPosition rightEnd = findPartitionRight(nextMove);
	//GridPosition rightEdge = findPartitionRightEdge(nextMove);
	int moveLength = (nextMove.col - leftEnd.col) + 1;
	int moveSpace = findRightMoveBuffer(nextMove);
	//int moveSpace = (rightEdge.col - rightEnd.col);
	if(moveSpace > moveLength){
		return true;
	}
	else{
		return false;
	}
}

// DELTA
bool canMoveHorizontaly(TravelerSegment nextMove){
	return canMoveLeft(nextMove) || canMoveRight(nextMove);
}

GridPosition findPartitionLeft(TravelerSegment nextMove){
	int i = 0;
	struct GridPosition partLeft;
	partLeft.row = nextMove.row;
	while((int)nextMove.col - (i + 1) >= 0){
		if(grid[nextMove.row][nextMove.col - (i + 1)] != SquareType::HORIZONTAL_PARTITION){
			partLeft.col = (nextMove.col - i);
			return partLeft;
		}
		i++;
	}
	partLeft.col = (nextMove.col - i);
	return partLeft;
}

GridPosition findPartitionRight(TravelerSegment nextMove){
	int i = 0;
	struct GridPosition partRight;
	partRight.row = nextMove.row;
	while(nextMove.col + (i + 1) < numCols){
		if(grid[nextMove.row][nextMove.col + (i + 1)] != SquareType::HORIZONTAL_PARTITION){
			partRight.col = (nextMove.col + i);
			return partRight;
		}
		i++;
	}
	partRight.col = (nextMove.col + i);
	return partRight;
}

GridPosition findPartitionLeftEdge(TravelerSegment nextMove){
	struct GridPosition partLeft = findPartitionLeft(nextMove);
	struct GridPosition leftEdge;
	leftEdge.row = partLeft.row;
	int i = 0;
	while((int)partLeft.col - (i + 1) >= 0){
		if(grid[partLeft.row][partLeft.col - (i + 1)] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	leftEdge.col = partLeft.col - i;
	return leftEdge;
}

GridPosition findPartitionRightEdge(TravelerSegment nextMove){
	struct GridPosition partRight = findPartitionRight(nextMove);
	struct GridPosition rightEdge;
	rightEdge.row = partRight.row;
	int i = 0;
	while(partRight.col + (i + 1) < numCols){
		if(grid[partRight.row][partRight.col + (i + 1)] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	rightEdge.col = partRight.col + i;
	return rightEdge;
}

int findLeftMoveBuffer(TravelerSegment nextMove){
	struct GridPosition partLeft = findPartitionLeft(nextMove);
	// struct GridPosition leftEdge;
	// leftEdge.col = partLeft.col;
	int i = 0;
	while((int)partLeft.col - (i + 1) >= 0){
		if(grid[partLeft.row][partLeft.col - (i + 1)] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	return i;
}

int findRightMoveBuffer(TravelerSegment nextMove){
	struct GridPosition partRight = findPartitionRight(nextMove);
	// struct GridPosition rightEdge;
	int i = 0;
	while(partRight.col + (i + 1) < numCols){
		if(grid[partRight.row][partRight.col + (i + 1)] == SquareType::FREE_SQUARE){
			i++;
		}
		else{
			break;
		}
	}
	return i;
}

void movePartitionLeft(TravelerSegment nextMove){
	struct GridPosition leftEnd = findPartitionLeft(nextMove);
	struct GridPosition rightEnd = findPartitionRight(nextMove);
	// struct GridPosition leftEdge = findPartitionLeftEdge(nextMove);
	int moveLength = ((int)rightEnd.col - (int)nextMove.col) + 1;
	// int bufferSize = ((int)leftEnd.col - (int)leftEdge.col);
	//int bufferSize = findUpperMoveBuffer(nextMove);
	for(int i = 0; i < moveLength; i++){
		// grid[leftEnd.row][leftEnd.col - (i + 1)] = SquareType::HORIZONTAL_PARTITION;
		// grid[rightEnd.row][rightEnd.col - i] = SquareType::FREE_SQUARE;
		if(grid[leftEnd.row][leftEnd.col - (i + 1)] == SquareType::FREE_SQUARE){
			grid[leftEnd.row][leftEnd.col - (i + 1)] = SquareType::HORIZONTAL_PARTITION;
		}
		if(grid[rightEnd.row][rightEnd.col - i] == SquareType::HORIZONTAL_PARTITION){
			grid[rightEnd.row][rightEnd.col - i] = SquareType::FREE_SQUARE;
		}
	}
}

void movePartitionRight(TravelerSegment nextMove){
	struct GridPosition rightEnd = findPartitionRight(nextMove);
	struct GridPosition leftEnd = findPartitionLeft(nextMove);
	// struct GridPosition rightEdge = findPartitionRightEdge(nextMove);
	int moveLength = ((int)nextMove.col - (int)leftEnd.col) + 1;
	//int bufferSize = findLowerMoveBuffer(nextMove);
	// int bufferSize = ((int)rightEdge.col - (int)rightEnd.col);
	for(int i = 0; i < moveLength; i++){
		if(grid[rightEnd.row][rightEnd.col + (i + 1)] == SquareType::FREE_SQUARE){
			grid[rightEnd.row][rightEnd.col + (i + 1)] = SquareType::HORIZONTAL_PARTITION;
		}
		if(grid[leftEnd.row][leftEnd.col + i] == SquareType::HORIZONTAL_PARTITION){
			grid[leftEnd.row][leftEnd.col + i] = SquareType::FREE_SQUARE;
		}
	}
}

void moveHorizontalPartition(TravelerSegment nextMove){
	if(canMoveLeft(nextMove) && canMoveRight(nextMove)){
		int choice = headsOrTails(engine);
		if(choice == 1){
			movePartitionLeft(nextMove);
		}
		else if(choice == 0){
			movePartitionRight(nextMove);
		}
		else{
			cout << "error selecting partition to move\n";
		}
	}
	else if(canMoveLeft(nextMove)){
		movePartitionLeft(nextMove);
	}
	else if(canMoveRight(nextMove)){
		movePartitionRight(nextMove);
	}
	else{
		std::cout << "can't move partition\n";
	}
}

void loadMoveList(vector<TravelerSegment> &nextMoveList, TravelerSegment &head){
	nextMoveList.clear();
	for(int i = 0; i < (int)Direction::NUM_DIRECTIONS; i++){
		Direction temp_dir = static_cast<Direction>(i);
		if(temp_dir != inverseDirection(head.dir)){
			TravelerSegment tempMove = directionalMove(head, temp_dir);
			if(isValid(tempMove) && isFree(tempMove)){
				nextMoveList.push_back(tempMove);
			}
			else if(isValid(tempMove) && congruentPartition(tempMove)){
				if(grid[tempMove.row][tempMove.col] == SquareType::VERTICAL_PARTITION && canMoveVerticaly(tempMove)){
					nextMoveList.push_back(tempMove);
				}
				else if(grid[tempMove.row][tempMove.col] == SquareType::HORIZONTAL_PARTITION && canMoveHorizontaly(tempMove)){
					nextMoveList.push_back(tempMove);
				}
			}
		}
	}
}

void moveTraveler(Traveler &runner, TravelerSegment newHead, int &moveCounter){
	runner.segmentList.push_front(newHead);
	if(!isExit(newHead) && isFree(newHead)){
		grid[newHead.row][newHead.col] = SquareType::TRAVELER;
	}
	moveCounter++;
	if(moveCounter % movesToGrow != 0){
		TravelerSegment tail = runner.segmentList.back();
		grid[tail.row][tail.col] = SquareType::FREE_SQUARE;
		runner.segmentList.pop_back();
	}
	//cout << "moving to: " << newHead.row << ", " << newHead.col << "\n";
}

bool changeDirection(Traveler &runner, Direction trajectory){
	if(runner.segmentList[0].dir == trajectory){
		return true;
	}
	return false;
}

Direction inverseDirection(Direction trajectory){
	switch(trajectory){
		case Direction::NORTH:
			return Direction::SOUTH;
			break;
		case Direction::WEST:
			return Direction::EAST;
			break;
		case Direction::SOUTH:
			return Direction::NORTH;
			break;
		case Direction::EAST:
			return Direction::WEST;
			break;
		default:
			return Direction::NUM_DIRECTIONS;
			break;
	}
}

TravelerSegment selectMove(vector<TravelerSegment> &nextMoveList){
	for(unsigned int i = 0; i < nextMoveList.size(); i++){
		if(isExit(nextMoveList[i])){
			return nextMoveList[i];
		}
	}
	unsigned int upperLimit = nextMoveList.size() - 1;
	uniform_int_distribution<unsigned int> movePicker(0, upperLimit);
	TravelerSegment selectedMove = nextMoveList[movePicker(engine)];
	return selectedMove;
	//return nextMoveList[movePicker(engine)];
}

void eraseTraveler(Traveler &runner){
	for(int i = (int)runner.segmentList.size() - 1; i >= 0; i--){
		TravelerSegment tempSeg = runner.segmentList[i];
		if(grid[tempSeg.row][tempSeg.col] == SquareType::TRAVELER){
			grid[tempSeg.row][tempSeg.col] = SquareType::FREE_SQUARE;
		}
		runner.segmentList.pop_back();
	}
	numTravelersDone++;
}

//lock section: Delta

void initializeLockGrid(){
	for(int i = 0; i < numRows; i++){
		lockGrid.push_back(vector<mutex>(numCols));
	}
}

void scheduleTravelerLocks(set<pair<int, int> > &lockSet, Traveler runner){
	TravelerSegment head = runner.segmentList.front();
	TravelerSegment tail = runner.segmentList.back();
	lockSet.insert(make_pair(head.row, head.col));
	for(int i = 0; i < (int)Direction::NUM_DIRECTIONS; i++){
		Direction temp_dir = static_cast<Direction>(i);
		if(temp_dir != inverseDirection(head.dir)){
			TravelerSegment tempMove = directionalMove(head, temp_dir);
			if(isValid(tempMove)){
				lockSet.insert(make_pair(tempMove.row, tempMove.col));
				if(verticalPartitionMap[tempMove.row][tempMove.col]){
					int n = tempMove.row - 1;
					while(n >= 0){
						if(verticalPartitionMap[n][tempMove.col]){
							lockSet.insert(make_pair(n, tempMove.col));
							n--;
						}
						else{
							break;
						}
					}
					int m = tempMove.row - 1;
					while(m < numRows){
						if(verticalPartitionMap[m][tempMove.col]){
							lockSet.insert(make_pair(m, tempMove.col));
							m++;
						}
						else{
							break;
						}
					}
				}
				if(horizontalPartitionMap[tempMove.row][tempMove.col]){
					int n = tempMove.col - 1;
					while(n >= 0){
						if(horizontalPartitionMap[tempMove.row][n]){
							lockSet.insert(make_pair(tempMove.row, n));
							n--;
						}
						else{
							break;
						}
					}
					int m = tempMove.row - 1;
					while(m < numCols){
						if(horizontalPartitionMap[tempMove.row][m]){
							lockSet.insert(make_pair(tempMove.row, m));
							m++;
						}
						else{
							break;
						}
					}
				}
			}
		}
	}
}

//locks the points in the set
void lockTheLockSet(set<pair<int, int> > &lockSet){
	for(int i = 0; i < numRows; i++){
		for(int j = 0; j < numCols; j++){
			pair<int, int> tempPair = make_pair(i, j);
			if(lockSet.count(tempPair) != 0){
				lockPair(tempPair);
			}
			else{
				continue;
			}
		}
	}
}

//unlocks the points in the set
void unlockTheLockSet(set<pair<int, int> > &lockSet){
	for(int i = 0; i < numRows; i++){
		for(int j = 0; j < numCols; j++){
			pair<int, int> tempPair = make_pair(i, j);
			if(lockSet.count(tempPair) != 0){
				unlockPair(tempPair);
			}
			else{
				continue;
			}
		}
	}
	lockSet.clear();
}

//locks a lock at the points in the pair
void lockPair(pair<int, int> lockPoint){
	lockGrid[lockPoint.first][lockPoint.second].lock();
}

//unlocks a lock at the points in the pair
void unlockPair(pair<int, int> lockPoint){
	lockGrid[lockPoint.first][lockPoint.second].unlock();
}

void initializeVerticalPartitionMap(){
	for(int i = 0; i < numRows; i++){
		verticalPartitionMap.push_back(vector<bool>(numCols, false));
	}
}

void initializeHorizontalPartitionMap(){
	for(int i = 0; i < numRows; i++){
		horizontalPartitionMap.push_back(vector<bool>(numCols, false));
	}
}

void writeVerticalPartitionMap(){
	initializeVerticalPartitionMap();
	for(int i = 0; i < numRows; i++){
		for(int j = 0; j < numCols; j++){
			if(verticalPartitionMap[i][j] == true){
				continue;
			}
			if(grid[i][j] == SquareType::VERTICAL_PARTITION){
				verticalPartitionMap[i][j] = true;
				writeMapUp(i, j);
				writeMapDown(i, j);
			}
		}
	}
}

void writeHorizontalPartitionMap(){
	initializeHorizontalPartitionMap();
	for(int i = 0; i < numRows; i++){
		for(int j = 0; j < numCols; j++){
			if(horizontalPartitionMap[i][j] == true){
				continue;
			}
			if(grid[i][j] == SquareType::HORIZONTAL_PARTITION){
				horizontalPartitionMap[i][j] = true;
				writeMapLeft(i, j);
				writeMapRight(i, j);
			}
		}
	}
}

void writeMapUp(int row, int col){
	for(int i = row - 1; i >= 0; i--){
		if(validPartitionZone(i, col)){
			verticalPartitionMap[i][col] = true;
		}
		else{
			break;
		}
	}
}
void writeMapDown(int row, int col){
	for(int i = row + 1; i < numRows; i++){
		if(validPartitionZone(i, col)){
			verticalPartitionMap[i][col] = true;
		}
		else{
			break;
		}
	}
}
void writeMapLeft(int row, int col){
	for(int j = col - 1; j >= 0; j--){
		if(validPartitionZone(row, j)){
			horizontalPartitionMap[row][j] = true;
		}
		else{
			break;
		}
	}
}
void writeMapRight(int row, int col){
	for(int j = col + 1; j < numCols; j++){
		if(validPartitionZone(row, j)){
			horizontalPartitionMap[row][j] = true;
		}
		else{
			break;
		}
	}
}



bool validPartitionZone(int row, int col){
	if(grid[row][col] == SquareType::FREE_SQUARE){
		return true;
	}
	else if(grid[row][col] == SquareType::VERTICAL_PARTITION){
		return true;
	}
	else if(grid[row][col] == SquareType::HORIZONTAL_PARTITION){
		return true;
	}
	else{
		return false;
	}
}


//------------------------------------------------------
#if 0
#pragma mark -
#pragma mark Generation Helper Functions
#endif
//------------------------------------------------------

GridPosition getNewFreePosition(void)
{
	GridPosition pos;

	bool noGoodPos = true;
	while (noGoodPos)
	{
		unsigned int row = rowGenerator(engine);
		unsigned int col = colGenerator(engine);
		if (grid[row][col] == SquareType::FREE_SQUARE)
		{
			pos.row = row;
			pos.col = col;
			noGoodPos = false;
		}
	}
	return pos;
}

Direction newDirection(Direction forbiddenDir)
{
	bool noDir = true;

	Direction dir = Direction::NUM_DIRECTIONS;
	while (noDir)
	{
		dir = static_cast<Direction>(segmentDirectionGenerator(engine));
		noDir = (dir==forbiddenDir);
	}
	return dir;
}


TravelerSegment newTravelerSegment(const TravelerSegment& currentSeg, bool& canAdd)
{
	TravelerSegment newSeg;
	switch (currentSeg.dir)
	{
		case Direction::NORTH:
			if (	currentSeg.row < numRows-1 &&
					grid[currentSeg.row+1][currentSeg.col] == SquareType::FREE_SQUARE)
			{
				newSeg.row = currentSeg.row+1;
				newSeg.col = currentSeg.col;
				newSeg.dir = newDirection(Direction::SOUTH);
				grid[newSeg.row][newSeg.col] = SquareType::TRAVELER;
				canAdd = true;
			}
			//	no more segment
			else
				canAdd = false;
			break;

		case Direction::SOUTH:
			if (	currentSeg.row > 0 &&
					grid[currentSeg.row-1][currentSeg.col] == SquareType::FREE_SQUARE)
			{
				newSeg.row = currentSeg.row-1;
				newSeg.col = currentSeg.col;
				newSeg.dir = newDirection(Direction::NORTH);
				grid[newSeg.row][newSeg.col] = SquareType::TRAVELER;
				canAdd = true;
			}
			//	no more segment
			else
				canAdd = false;
			break;

		case Direction::WEST:
			if (	currentSeg.col < numCols-1 &&
					grid[currentSeg.row][currentSeg.col+1] == SquareType::FREE_SQUARE)
			{
				newSeg.row = currentSeg.row;
				newSeg.col = currentSeg.col+1;
				newSeg.dir = newDirection(Direction::EAST);
				grid[newSeg.row][newSeg.col] = SquareType::TRAVELER;
				canAdd = true;
			}
			//	no more segment
			else
				canAdd = false;
			break;

		case Direction::EAST:
			if (	currentSeg.col > 0 &&
					grid[currentSeg.row][currentSeg.col-1] == SquareType::FREE_SQUARE)
			{
				newSeg.row = currentSeg.row;
				newSeg.col = currentSeg.col-1;
				newSeg.dir = newDirection(Direction::WEST);
				grid[newSeg.row][newSeg.col] = SquareType::TRAVELER;
				canAdd = true;
			}
			//	no more segment
			else
				canAdd = false;
			break;
		
		default:
			canAdd = false;
	}
	
	return newSeg;
}

void generateWalls(void)
{
	const unsigned int NUM_WALLS = (numCols+numRows)/4;

	//	I decide that a wall length  cannot be less than 3  and not more than
	//	1/4 the grid dimension in its Direction
	const unsigned int MIN_WALL_LENGTH = 3;
	const unsigned int MAX_HORIZ_WALL_LENGTH = numCols / 3;
	const unsigned int MAX_VERT_WALL_LENGTH = numRows / 3;
	const unsigned int MAX_NUM_TRIES = 20;

	bool goodWall = true;
	
	//	Generate the vertical walls
	for (unsigned int w=0; w< NUM_WALLS; w++)
	{
		goodWall = false;
		
		//	Case of a vertical wall
		if (headsOrTails(engine))
		{
			//	I try a few times before giving up
			for (unsigned int k=0; k<MAX_NUM_TRIES && !goodWall; k++)
			{
				//	let's be hopeful
				goodWall = true;
				
				//	select a column index
				unsigned int HSP = numCols/(NUM_WALLS/2+1);
				unsigned int col = (1+ unsignedNumberGenerator(engine)%(NUM_WALLS/2-1))*HSP;
				unsigned int length = MIN_WALL_LENGTH + unsignedNumberGenerator(engine)%(MAX_VERT_WALL_LENGTH-MIN_WALL_LENGTH+1);
				
				//	now a random start row
				unsigned int startRow = unsignedNumberGenerator(engine)%(numRows-length);
				for (unsigned int row=startRow, i=0; i<length && goodWall; i++, row++)
				{
					if (grid[row][col] != SquareType::FREE_SQUARE)
						goodWall = false;
				}
				
				//	if the wall first, add it to the grid
				if (goodWall)
				{
					for (unsigned int row=startRow, i=0; i<length && goodWall; i++, row++)
					{
						grid[row][col] = SquareType::WALL;
					}
				}
			}
		}
		// case of a horizontal wall
		else
		{
			goodWall = false;
			
			//	I try a few times before giving up
			for (unsigned int k=0; k<MAX_NUM_TRIES && !goodWall; k++)
			{
				//	let's be hopeful
				goodWall = true;
				
				//	select a column index
				unsigned int VSP = numRows/(NUM_WALLS/2+1);
				unsigned int row = (1+ unsignedNumberGenerator(engine)%(NUM_WALLS/2-1))*VSP;
				unsigned int length = MIN_WALL_LENGTH + unsignedNumberGenerator(engine)%(MAX_HORIZ_WALL_LENGTH-MIN_WALL_LENGTH+1);
				
				//	now a random start row
				unsigned int startCol = unsignedNumberGenerator(engine)%(numCols-length);
				for (unsigned int col=startCol, i=0; i<length && goodWall; i++, col++)
				{
					if (grid[row][col] != SquareType::FREE_SQUARE)
						goodWall = false;
				}
				
				//	if the wall first, add it to the grid
				if (goodWall)
				{
					for (unsigned int col=startCol, i=0; i<length && goodWall; i++, col++)
					{
						grid[row][col] = SquareType::WALL;
					}
				}
			}
		}
	}
}


void generatePartitions(void)
{
	const unsigned int NUM_PARTS = (numCols+numRows)/4;

	//	I decide that a partition length  cannot be less than 3  and not more than
	//	1/4 the grid dimension in its Direction
	const unsigned int MIN_PARTITION_LENGTH = 3;
	const unsigned int MAX_HORIZ_PART_LENGTH = numCols / 3;
	const unsigned int MAX_VERT_PART_LENGTH = numRows / 3;
	const unsigned int MAX_NUM_TRIES = 20;

	bool goodPart = true;

	for (unsigned int w=0; w< NUM_PARTS; w++)
	{
		goodPart = false;
		
		//	Case of a vertical partition
		if (headsOrTails(engine))
		{
			//	I try a few times before giving up
			for (unsigned int k=0; k<MAX_NUM_TRIES && !goodPart; k++)
			{
				//	let's be hopeful
				goodPart = true;
				
				//	select a column index
				unsigned int HSP = numCols/(NUM_PARTS/2+1);
				unsigned int col = (1+ unsignedNumberGenerator(engine)%(NUM_PARTS/2-2))*HSP + HSP/2;
				unsigned int length = MIN_PARTITION_LENGTH + unsignedNumberGenerator(engine)%(MAX_VERT_PART_LENGTH-MIN_PARTITION_LENGTH+1);
				
				//	now a random start row
				unsigned int startRow = unsignedNumberGenerator(engine)%(numRows-length);
				for (unsigned int row=startRow, i=0; i<length && goodPart; i++, row++)
				{
					if (grid[row][col] != SquareType::FREE_SQUARE)
						goodPart = false;
				}
				
				//	if the partition is possible,
				if (goodPart)
				{
					//	add it to the grid and to the partition list
					SlidingPartition part;
					part.isVertical = true;
					for (unsigned int row=startRow, i=0; i<length && goodPart; i++, row++)
					{
						grid[row][col] = SquareType::VERTICAL_PARTITION;
						GridPosition pos = {row, col};
						part.blockList.push_back(pos);
					}
				}
			}
		}
		// case of a horizontal partition
		else
		{
			goodPart = false;
			
			//	I try a few times before giving up
			for (unsigned int k=0; k<MAX_NUM_TRIES && !goodPart; k++)
			{
				//	let's be hopeful
				goodPart = true;
				
				//	select a column index
				unsigned int VSP = numRows/(NUM_PARTS/2+1);
				unsigned int row = (1+ unsignedNumberGenerator(engine)%(NUM_PARTS/2-2))*VSP + VSP/2;
				unsigned int length = MIN_PARTITION_LENGTH + unsignedNumberGenerator(engine)%(MAX_HORIZ_PART_LENGTH-MIN_PARTITION_LENGTH+1);
				
				//	now a random start row
				unsigned int startCol = unsignedNumberGenerator(engine)%(numCols-length);
				for (unsigned int col=startCol, i=0; i<length && goodPart; i++, col++)
				{
					if (grid[row][col] != SquareType::FREE_SQUARE)
						goodPart = false;
				}
				
				//	if the wall first, add it to the grid and build SlidingPartition object
				if (goodPart)
				{
					SlidingPartition part;
					part.isVertical = false;
					for (unsigned int col=startCol, i=0; i<length && goodPart; i++, col++)
					{
						grid[row][col] = SquareType::HORIZONTAL_PARTITION;
						GridPosition pos = {row, col};
						part.blockList.push_back(pos);
					}
					partitionList.push_back(part);
				}
			}
		}
	}
}

