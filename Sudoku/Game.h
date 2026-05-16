/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Game.h
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .h file for the Game class, also containing an enumerator for the current GameState
=================================================================================================================== */
#ifndef GAME_H
#define GAME_H

// Libraries and other include files
#include "Board.h"
#include "Highlighter.h"
#include "Notepad.h"
#include "GameMemory.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <string>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Image.hpp>

// Enumerator for the current state of the game
enum GameState { TITLE_SCREEN, GAME_SCREEN, OVER_SCREEN, EXIT_SCREEN };

// Game class
class Game {
public:
	/*
	*	This is the default constructor for the game class
	*/
	Game();

	/*
	*	This is the default destructor for the game class
	*/
	~Game();

	/*
	*	Plays the sudoku game, based on the current gameState
	*/
	void playGame();

	/*
	*	Processes the title screen
	*/
	void processTitleScreen();

	/*
	*	Handles all of the user input events while on the title screen
	*/
	void handleTitleEvents();

	/*
	*	Processes the main game screen
	*/
	void processGameScreen();

	/*
	*	Handles all of the user input events while on the main game screen
	*/
	void handleGameEvents();

	/*
	*	Processes the game over screen
	*/
	void processOverScreen();

	/*
	*	Handles all of the user input events while on the game over screen
	*/
	void handleOverEvents();

	/*
	*	Tests if there are any duplicate numbers in any row, column, or 3x3 grid and
	*	changes the boards color code grid accordingly (red (r) = doesn't fit) (blue (b) = does fit) 
	*/
	void testIfPiecesFit();

	/*
	*	Tests if there are any matching numbers in any row, column, or 3x3 grid and
	*	changes the notepad color code grid accordingly (red (r) = doesn't fit) (green (g) = does fit)
	*/
	void testIfNotesFit();

	/*
	*	Tests to see if the board is complete (ie. no duplicates in rows, columns, and 3x3s) and all color codes are either 'b' or 'k' (NOT 'r')
	* 
	*	@return bool true - If the board is complete
	*	@return bool false - If the board is incomplete
	*/
	bool testIfBoardComplete();

	/*
	*	Returns the amount of time played via a string
	* 
	*	@return gameTime
	*/
	std::string getPlayTime();

private:
	sf::RenderWindow gameWindow; // For the window to display the game on
	sf::Image icon;
	sf::Font gameFont; // For the font used throughout the game
	GameState currentGameState; // For the current state of the game
	Difficulty currentDifficulty; // For the current difficulty of the game
	Board* gameBoard; // For the sudoku game board
	Highlighter* highlighter; // For the highlighter piece moving on the board
	Notepad* notepad; // For the notepad below the sudoku board
	GameMemory* gameMemory; // For the memory of the board states
	int hintCount; // For the number of hints to be provided
	bool isNotepadOpen; // To indicate if the notepad is open or not
	bool isPauseScreenOpen; // To indicate if the pause screen is open or not
	sf::Clock gameClock; // For the current running game time
	std::string finalTime; // To display the length of time spent on the board

	/*
	*	Handles all events occuring on the main game game screen when the game board is active
	* 
	*	@param sf::Event event - The keyboard event datatype being used
	*/
	void handleBoardEvents(sf::Event event);

	/*
	*	Handles all events occuring on the main game game screen when the notepad is active
	*
	*	@param sf::Event event - The keyboard event datatype being used
	*/
	void handleNoteEvents(sf::Event event);

	/*
	*	Draws the 9x9 sudoku board onto the window
	*/
	void drawBoard();

	/*
	*	Draws the highlighter piece onto the board
	*/
	void drawHighlighter();

	/*
	*	Draws the individual color-coded numbers onto the board
	*/
	void drawMainBoardNumbers();

	/*
	*	Draws a 1 - 9 digit blank 'Notepad' and it's numbers right below the main game board
	*/
	void drawNotepad();

	/*
	*	Draws the notes used on the notepad as smaller numbers to go on top of the game board when a spot is empty
	*/
	void drawNotesOnBoard();

	/*
	*	Draws the title screen overlays
	*/
	void drawTitleScreenOverlays();

	/*
	*	Draws the game screen overlays
	*/
	void drawGameScreenOverlays();

	/*
	*	Draws the game over screen overlays
	*/
	void drawGameOverOverlays();

	/*
	*	Draws the pause screen overlay
	*/
	void drawPauseScreen();
};
#endif