/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Notepad.h
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .h file for the Notepad class
=================================================================================================================== */
#ifndef NOTEPAD_H
#define NOTEPAD_H

// Include files
#include "Constants.h"

// Class for a Sudoku Notepad
class Notepad {
public:
	/*
	*	Default constructor for the Notepad
	*/
	Notepad();

	/*
	*	Toggles the notepadColorGrid color code depending on the location (row, column) and the number being toggled (1 - 9)
	* 
	*	@param int row - The current row of the color code being toggled
	*	@param int col - The current column of the color code being toggled
	*	@param int noteNumber - The number (1 - 9) that's being toggled
	*	@param char colorCode - The color code being set
	*/
	void setSpecificColorCode(int row, int col, int noteNumber, char colorCode);

	/*
	*	Set's the whole notepad color code grid to the provided copy from the given parameter
	* 
	*	@param char gridIn[][][] - The color code grid copy being provided
	*/
	void setWholeNotepadColorCode(char gridIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]);

	/*
	*	Resets the color codes of the notes at the given location
	* 
	*	@param int row - The current row that color codes are being made invisible
	*	@param int col - The current column that the color codes are being made invisible
	*/
	void resetCurrentPositionColorCodes(int row, int col);

	/*
	*	Resets the etire notepads color codes to 'b'
	*/
	void resetWholeNotepadColorCode();

	/*
	*	Returns a copy of the numbers grid containing (1 - 9) based on the location (row, column) being found
	* 
	*	@param int row - The row of the 'notes' being found
	*	@param int col - The column of the 'notes' being found
	*	@param int notesOut[] - The array of numbers found at that row/column
	*/
	void getCurrentPositionNotes(int row, int col, int notesOut[BOARD_SIZE]) const;
	
	/*
	*	Returns a copy of the current color code grid via the given parameter
	* 
	*	@param char notesOut[][][] - The color code grid copy going out 
	*/
	void getAllColorCodes(char notesOut[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]) const;

	/*
	*	Returns a copy of the color code grid
	* 
	*	@param int row - The row of the color code array being found
	*	@param int col - The column of the color code array being found
	*	@param char colorCodesOut[] - The array of color codes foud at that row/column
	*/
	void getCurrentPositionColorCodes(int row, int col, char colorCodesOut[BOARD_SIZE]) const;

	/*
	*	Gets a specific color code from a given row, column, and note number
	* 
	*	@param int row - The row of the color code being found
	*	@param int col - The column of the color code being found
	*	@param int noteNumber - The note number of the color code being found
	*/
	char getSpecificColorCode(int row, int col, int noteNumber) const;

private:
	int notepadNumberGrid[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]; // A 3-D Grid to hold rows and columns with numbers 1-9 in them as 'notes' taken for the sudoku game
	char notepadColorGrid[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]; // A 3-D Grid to hold the color codes of the notes, 'b' = blank, 'r' = piece doesn't fit, 'g' = piece fits
};

#endif