/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Highlighter.h
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .h file for the Highlighter class, which also includes the struct for a column and row
				location to be used for the sudoku board.
=================================================================================================================== */
#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

// Structure for the location (row and column) of the highlighter
struct Location {
	int row;
	int column;
};

// Highlighter class
class Highlighter {
public:
	/*
	*	Default constructor for the highlighter class
	*/
	Highlighter();

	/*
	*	Alternate constructor for setting a new highlighter to a specific location
	* 
	*	@param Location location - The location (row, column) of the new highlighter being created
	*/
	Highlighter(Location location);

	/*
	*	Set's the location of the highlighter via given parameters
	* 
	*	@param int row - The row that the highlighter location is being set to
	*	@param int col - The column that the highlighter location is being set to
	*/
	void setLocation(int row, int col);

	/*
	*	Moves the highlighter to the left on the game board
	*/
	void moveLeft();

	/*
	*	Moves the highlighter to the right on the game board
	*/
	void moveRight();

	/*
	*	Moves the highlighter up on the game board
	*/
	void moveUp();

	/*
	*	Moves the highlighter down on the game board
	*/
	void moveDown();

	/*
	*	Gets the current location of the highlighter on the game screen
	* 
	*	@return Location currentLocation - The current location of the highlighter
	*/
	Location getLocation() const;

	/*
	*	Checks to see if the move that the highlighter just made is a valid move or not
	* 
	*	@return bool true - If the new location is within bounds of the game board
	*	@return bool false - If the new location is out of bounds of the game board
	*/
	bool isMoveValid() const;

private:
	Location currentLocation; // For the current location (row, column) of the highlighter on the game board
};

#endif