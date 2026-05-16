/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			GameMemory.h
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .h file for the GameMemory class, used to create copies of the board to revert back to.
=================================================================================================================== */

#ifndef GAMEMEMORY_H
#define GAMEMEMORY_H

// Include files
#include "Constants.h"

// Struct for a node containing different board elements
struct Node {
	int numberGridMem[BOARD_SIZE][BOARD_SIZE];
	char colorGridMem[BOARD_SIZE][BOARD_SIZE];
	char noteColorMem[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	Node* next;
};

// GameMemory class
class GameMemory {
public:
	/*
	*	Default constructor for the GameMemory class
	*/
	GameMemory();

	/*
	*	Destructor for the GameMemory class
	*/
	~GameMemory();

	/*
	*	Loads the current board state into memory
	* 
	*	@param int numberGridIn - The board number grid being added to memory
	*	@param char colorGridIn - The board color grid being added to memory
	*	@param char noteColorIn - The notes color grid being added to memory
	*/
	void createNewMemory(int numberGridIn[BOARD_SIZE][BOARD_SIZE], char colorGridIn[BOARD_SIZE][BOARD_SIZE], char noteColorIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Removes the current board state from memory and overwrites the current board with the previous board state
	* 
	*	@param int numberGridOut - The board number grid being removed from memory
	*	@param char colorGridOut - The board color grid being removed from memory
	*	@param char noteColorOut - The notes color grid being removed from memory
	*/
	void deleteMemory(int numberGridOut[BOARD_SIZE][BOARD_SIZE], char colorGridOut[BOARD_SIZE][BOARD_SIZE], char noteColorOut[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Tests if there have been any changes made to the current board state
	*
	*	@param int numberGridOut - The board number grid being tested
	*	@param char colorGridOut - The board color grid being tested
	*	@param char noteColorOut - The notes color grid being tested
	*/
	bool testIfChangesMade(int numberGridIn[BOARD_SIZE][BOARD_SIZE], char colorGridIn[BOARD_SIZE][BOARD_SIZE], char noteColorIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Deletes all the board states currently in memory
	*/
	void removeAllMemory();
private:
	Node* head;
};

#endif