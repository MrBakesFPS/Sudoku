/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Board.h
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is .h file for the Board class, of which also contains an enumerator for the different
				difficulty types used to determine how many empty spaces are on each new creation of the board.
=================================================================================================================== */
#ifndef BOARD_H
#define BOARD_H

// Libraries and other include files
#include "Constants.h"

// Enumerator for the different difficulty types of the game
enum Difficulty { EASY, NORMAL, HARD, EXPERT, BEGINNER, IMPOSSIBLE };

// Board class
class Board {
public:
	/*
	*	Default Board constructor
	*/
	Board();
	
	/*
	*	Initializes a brand new, randomized, solvable, sudoku board
	*/
	bool initializeNewBoard();

	/*
	*	Creates a random set of numbers to be visible on the board while playing the game based on a given difficulty parameter
	*
	*	@param Difficulty difficulty - The current difficulty to help determine the number of visible numbers on the board
	*/
	void createSolidPieces(Difficulty difficulty);

	/*
	*	Tests if the current board that's being created is uniquely solvable
	* 
	*	@return bool true - If the current board where numbers are being deleted is a uniquely solvable board
	*	@return bool false - If the current board where numbers are being deleted can be solved in more than one way
	*/
	bool isCurrentBoardUnique();

	/*
	*	Recursive function that checks to see how many times the current board can be solved
	* 
	*	@param int boardBeingSolved[][] - The board grid that's currently being solved
	*	
	*	@return int uniqueBoardSolves - The number of times the board was solved (2 times is max)
	*/
	int uniqueBoardSolves(int boardBeingSolved[BOARD_SIZE][BOARD_SIZE]);

	/*
	*   Solves the current board and stores the result in the given output grid
	*
	*   @param int solvedGrid[][] - Output grid that will be filled with the solution
	*
	*   @return bool true  - If a solution was found
	*   @return bool false - If the board has no solution
	*/
	bool createSolvedBoard(int solvedGrid[BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Provides a hint to the current board
	*/
	void hint();

	/*
	*	Sets the numberGrid to a new grid from the given parameters
	* 
	*	@param int gridIn[][] - The new number grid being provided
	*/
	void setNumberGrid(int gridIn[BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Sets the colorGrid to a new grid from the given parameters
	* 
	*	@param char gridIn[][] - The new color grid being provided
	*/
	void setColorGrid(char gridIn[BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Tests if a number that's being placed actually fits at a specific location on the sudoku board
	* 
	*	@param imt numberGridIn[][] - The board that's currenty being tested for the new number to fit
	*	@param int newNum - The new number being tested
	*	@param int currRow - The current row where the new number is being placed
	*	@param int currCol - The current col where the new number is being placed
	* 
	*	@return bool false - If the new number does not fit on the board at the given location
	*	@return bool true - If the new number does fit on the board at the given location
	*/
	bool testIfNewPieceFits(int numberGridIn[BOARD_SIZE][BOARD_SIZE], int newNum, int currRow, int currCol) const;

	/*
	*	Returns a copy of the numberGrid via the given parameters
	* 
	*	@param int gridOut[][] - The grid copy that's grabbing the number grid
	*/
	void getNumberGrid(int gridOut[BOARD_SIZE][BOARD_SIZE]) const;

	/*
	*	Returns a copy of the colorGrid via the given parameters
	* 
	*	@param char gridOut[][] - The grid copy that's grabbing the color grid
	*/
	void getColorGrid(char gridOut[BOARD_SIZE][BOARD_SIZE]) const;

	/*
	*	Restarts the current number grid back to it's original unsolved, untouched state
	*/
	void restartCurrentNumberGrid();

	/*
	*	Generates a 17 clue board from a resource file
	*/
	void seventeenClueBoard();

	/*
	*	Mixes the current board by calling random swap functions
	*/
	void mixBoard();

	/*
	*	Swaps the top 3 rows with the bottom 3
	*/
	void swap1();

	/*
	*	Swaps the left 3 columns with the columns 3
	*/
	void swap2();

	/*
	*	Swaps the left 3 columns with the middle 3 columns
	*/
	void swap3();

	/*
	*	Swaps the middle 3 columns with the right 3 columns
	*/
	void swap4();

	/*
	*	Swaps the top 3 rows with the middle 3 rows
	*/
	void swap5();

	/*
	*	Swaps the middle 3 rows with the bottom 3 rows
	*/
	void swap6();

	/*
	*	Swaps by mirror over the horizontal axis
	*/
	void swap7();

	/*
	*	Swaps by mirror over the verticle axis
	*/
	void swap8();

	/*
	*	Rotates the board left
	*/
	void swap9();

	/*
	*	Rotates the board right
	*/
	void swap10();

	/*
	*	Swaps a random row or column with it's row 3 or column 3 specific neighbors
	*/
	void swap11();

private:
	int numberGrid[BOARD_SIZE][BOARD_SIZE]; // For the numbers on the sudoku board
	int solvedBoard[BOARD_SIZE][BOARD_SIZE]; // For the numbers on the sudoku board
	char colorGrid[BOARD_SIZE][BOARD_SIZE]; // For the color codes of the numbers on the sudoku board
};

#endif