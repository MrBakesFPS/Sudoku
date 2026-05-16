/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Highlighter.cpp
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .cpp file for the Highlighter class, used for the highlighter on the sudoku board.
=================================================================================================================== */

// Include files
#include "Highlighter.h"
#include "constants.h"

// Default Highlighter constructor
Highlighter::Highlighter()
{
	currentLocation = { LAYOUT_BOARD_TOP, LAYOUT_BOARD_LEFT };
}

// Alternate Highlighter constructor
Highlighter::Highlighter(Location location)
{
	setLocation(location.row, location.column);
}

// setLocation function
void Highlighter::setLocation(int row, int col)
{
	currentLocation = { row, col };
}

// moveLeft function
void Highlighter::moveLeft()
{
	currentLocation = { currentLocation.row, currentLocation.column - BLOCK_SIZE_PIXELS };
}

// moveRight function
void Highlighter::moveRight()
{
	currentLocation = { currentLocation.row, currentLocation.column + BLOCK_SIZE_PIXELS };
}

// moveUp function
void Highlighter::moveUp()
{
	currentLocation = { currentLocation.row - BLOCK_SIZE_PIXELS, currentLocation.column };
}

// moveDown function
void Highlighter::moveDown()
{
	currentLocation = { currentLocation.row + BLOCK_SIZE_PIXELS, currentLocation.column };
}

// getLocation function
Location Highlighter::getLocation() const
{
	return currentLocation;
}

// isMoveValid function
bool Highlighter::isMoveValid() const
{
	if (currentLocation.row < LAYOUT_BOARD_TOP || currentLocation.row >= LAYOUT_BOARD_TOP + (BOARD_SIZE * BLOCK_SIZE_PIXELS)
		|| currentLocation.column < LAYOUT_BOARD_LEFT || currentLocation.column >= LAYOUT_BOARD_LEFT + (BOARD_SIZE * BLOCK_SIZE_PIXELS))
		return false;
	else
		return true;
}