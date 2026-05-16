/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Notepad.cpp
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .cpp file for the Notepad class, used for the notepad on the sudoku board.
=================================================================================================================== */

// Include files
#include "Notepad.h"
#include "Constants.h"

// Default Notepad constructor
Notepad::Notepad()
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			for (int k = 0; k < BOARD_SIZE; k++) {
				notepadNumberGrid[i][j][k] = k + 1;
				notepadColorGrid[i][j][k] = 'b';
			}
		}
	}
}

// setSpecificColorCode function
void Notepad::setSpecificColorCode(int row, int col, int noteNumber, char colorCode)
{
	notepadColorGrid[row][col][noteNumber - 1] = colorCode;

}

// setWholeNotepadColorCode function
void Notepad::setWholeNotepadColorCode(char gridIn[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			for (int k = 0; k < BOARD_SIZE; k++) {
				notepadColorGrid[i][j][k] = gridIn[i][j][k];
			}
		}
	}
}

// resetCurrentPositionColorCodes function
void Notepad::resetCurrentPositionColorCodes(int row, int col)
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		notepadColorGrid[row][col][i] = 'b';
	}
}

// resetWholeNotepadColorCode function
void Notepad::resetWholeNotepadColorCode()
{
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			for (int notePosition = 0; notePosition < BOARD_SIZE; notePosition++) {
				notepadColorGrid[row][col][notePosition] = 'b';
			}
		}
	}
}

// getCurrentPositionNotes function
void Notepad::getCurrentPositionNotes(int row, int col, int notesOut[BOARD_SIZE]) const
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (notepadColorGrid[row][col][i] == 'b')
			notesOut[i] = 0;
		else
			notesOut[i] = notepadNumberGrid[row][col][i];
	}
}

// getAllColorCodes function
void Notepad::getAllColorCodes(char notesOut[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]) const
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			for (int k = 0; k < BOARD_SIZE; k++) {
				notesOut[i][j][k] = notepadColorGrid[i][j][k];
			}
		}
	}
}

// getCurrentPositionColorCodes function
void Notepad::getCurrentPositionColorCodes(int row, int col, char colorCodesOut[BOARD_SIZE]) const
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		colorCodesOut[i] = notepadColorGrid[row][col][i];
	}
}

// getSpecificColorCode function
char Notepad::getSpecificColorCode(int row, int col, int noteNumber) const
{
	return notepadColorGrid[row][col][noteNumber - 1];
}