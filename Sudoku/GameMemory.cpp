/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			GameMemory.cpp
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .cpp file for the GameMemory class, used to create copies of the board to revert back to.
=================================================================================================================== */

// Include files
#include "GameMemory.h"
#include "Constants.h"

// Default GameMemory constructor
GameMemory::GameMemory()
{
	head = nullptr;
}

// Destructor
GameMemory::~GameMemory()
{
	removeAllMemory();
}

// createNewMemory function
void GameMemory::createNewMemory(int numberGridIn[BOARD_SIZE][BOARD_SIZE], char colorGridIn[BOARD_SIZE][BOARD_SIZE], char noteColorIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE])
{
	// Create the new node
	Node* newNode = new Node;

	// Create memory of the current board state
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			newNode->numberGridMem[i][j] = numberGridIn[i][j];
			newNode->colorGridMem[i][j] = colorGridIn[i][j];
			for (int k = 0; k < BOARD_SIZE; k++) {
				newNode->noteColorMem[i][j][k] = noteColorIn[i][j][k];
			}
		}
	}

	// Insert the new node into memory
	newNode->next = head;
	head = newNode;
	return;
}

// deleteMemory function
void GameMemory::deleteMemory(int numberGridOut[BOARD_SIZE][BOARD_SIZE], char colorGridOut[BOARD_SIZE][BOARD_SIZE], char noteColorOut[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE])
{
	// If head is null
	if (head == nullptr || head->next == nullptr)
		return;

	// Create the node to delete from beginning of list
	Node* deleteNode = head;

	// Make the head node the previous board state
	head = head->next;

	// Delete the delete node
	delete deleteNode;

	// Overwrite the current board state with the previous board state
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			numberGridOut[i][j] = head->numberGridMem[i][j];
			colorGridOut[i][j] = head->colorGridMem[i][j];
			for (int k = 0; k < BOARD_SIZE; k++) {
				noteColorOut[i][j][k] = head->noteColorMem[i][j][k];
			}
		}
	}
}

// testIfChangesMade function
bool GameMemory::testIfChangesMade(int numberGridIn[BOARD_SIZE][BOARD_SIZE], char colorGridIn[BOARD_SIZE][BOARD_SIZE], char noteColorIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE])
{
	if (head == nullptr)
		return false;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (colorGridIn[i][j] != head->colorGridMem[i][j])
				return true;
			if (numberGridIn[i][j] != head->numberGridMem[i][j])
				return true;
			for (int k = 0; k < BOARD_SIZE; k++) {
				if (noteColorIn[i][j][k] != head->noteColorMem[i][j][k])
					return true;
			}
		}
	}
	return false;
}

// removeAllMemory function
void GameMemory::removeAllMemory()
{
	// Create a node to delete
	Node* deleteNode = head;

	// While the list is not empty
	while (head != nullptr) {
		// Make head be the previous board state
		head = head->next;

		// Delete the delete node
		delete deleteNode;

		// Make the delete node the head node
		deleteNode = head;
	}
}