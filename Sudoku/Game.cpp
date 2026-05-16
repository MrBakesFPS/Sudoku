/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Game.cpp
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the .cpp file for the Game class, used to create and run the sudoku game.
=================================================================================================================== */

// Include files
#include "Game.h"
#include "Highlighter.h"
#include "Board.h"
#include "Notepad.h"
#include "GameMemory.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/WindowStyle.hpp>

// Default Game constructor
Game::Game()
{
	// The creation of the game window
	gameWindow.create(
		sf::VideoMode(LAYOUT_WINDOW_WIDTH, LAYOUT_WINDOW_HEIGHT),
		"Sudoku",
		sf::Style::Titlebar | sf::Style::Close
	);

	if (icon.loadFromFile("icon.png"))
		gameWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	// For the loading of the font from file
	std::cout << "Loading font..." << std::endl;
	if (!gameFont.loadFromFile("JumpersCondensedItalic-d9zq7.ttf"))
		std::cout << "ERROR: Font could not be found..." << std::endl;
	else
		std::cout << "SUCCESS: Font loaded!" << std::endl;

	currentGameState = TITLE_SCREEN;

	currentDifficulty = NORMAL;

	gameBoard = new Board;

	highlighter = new Highlighter;

	notepad = new Notepad;

	gameMemory = new GameMemory;

	hintCount = 0;

	isNotepadOpen = false;

	isPauseScreenOpen = false;

	gameClock.restart();

	finalTime = "";
}

// Default game destructor
Game::~Game()
{
	if (gameBoard != nullptr)
		delete gameBoard;
	if (highlighter != nullptr)
		delete highlighter;
	if (notepad != nullptr)
		delete notepad;
	if (gameMemory != nullptr)
		delete gameMemory;
}

// playGame function
void Game::playGame()
{
	while (currentGameState != EXIT_SCREEN)
	{
		if (currentGameState == TITLE_SCREEN)
			processTitleScreen();
		else if (currentGameState == GAME_SCREEN)
			processGameScreen();
		else if (currentGameState == OVER_SCREEN)
			processOverScreen();
	}
	gameWindow.close();
}

// processTitleScreen function
void Game::processTitleScreen()
{
	// While the game window is open and on the title screen
	while (gameWindow.isOpen() && currentGameState == TITLE_SCREEN)
	{
		// Handles user input events
		handleTitleEvents();

		// Drawing the new frame
		gameWindow.clear();
		drawTitleScreenOverlays();
		if (isPauseScreenOpen == true)
			drawPauseScreen();
		gameWindow.display();
	}
}

// handleTitleEvents function
void Game::handleTitleEvents()
{
	// For keyboard input events from the player
	sf::Event event;
	while (gameWindow.pollEvent(event) && currentGameState == TITLE_SCREEN)
	{
		// For the closing of the game screen
		if (event.type == sf::Event::Closed)
			currentGameState = EXIT_SCREEN;
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			isPauseScreenOpen = !isPauseScreenOpen;
		}
		if (isPauseScreenOpen == false) {
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
				currentGameState = GAME_SCREEN;
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
			{
				if (currentDifficulty == EASY)
					currentDifficulty = NORMAL;
				else if (currentDifficulty == NORMAL)
					currentDifficulty = HARD;
				else if (currentDifficulty == HARD)
					currentDifficulty = EXPERT;
				else if (currentDifficulty == EXPERT)
					currentDifficulty = IMPOSSIBLE;
				else if (currentDifficulty == IMPOSSIBLE)
					currentDifficulty = BEGINNER;
				else if (currentDifficulty == BEGINNER)
					currentDifficulty = EASY;
			}
		}
	}
}

// processGameScreen function
void Game::processGameScreen()
{
	// Initializes new gameboard, highlighter, and notepad pointers
	if (gameBoard == nullptr)
		gameBoard = new Board;
	if (highlighter == nullptr)
		highlighter = new Highlighter;
	if (notepad == nullptr)
		notepad = new Notepad;
	if (gameMemory == nullptr)
		gameMemory = new GameMemory;

	// Sets the notepad to closed on game start
	isNotepadOpen = false;

	// Generate new board
	gameBoard->createSolidPieces(currentDifficulty);

	// Copy first board state for the initializing of the games memory
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE];
	char copyNoteColor[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	gameBoard->getNumberGrid(copyNumGrid);
	gameBoard->getColorGrid(copyColorGrid);
	notepad->getAllColorCodes(copyNoteColor);

	// Initialize the game memory
	gameMemory->createNewMemory(copyNumGrid, copyColorGrid, copyNoteColor);

	// Initialize the hint count
	hintCount = 3;

	// Reset the game clock
	gameClock.restart();

	// While the game window is open and on the game screen
	while (gameWindow.isOpen() && currentGameState == GAME_SCREEN)
	{
		// Handles user input events
		handleGameEvents();
		
		// If currentGameState hasn't changed from the user input
		if (currentGameState == GAME_SCREEN)
		{
			// Test if the numbers and notes that're currently on the board don't match others in the same row, column, or 3x3 square
			testIfPiecesFit();
			testIfNotesFit();

			gameBoard->getNumberGrid(copyNumGrid);
			gameBoard->getColorGrid(copyColorGrid);
			notepad->getAllColorCodes(copyNoteColor);
			if (gameMemory->testIfChangesMade(copyNumGrid, copyColorGrid, copyNoteColor))
				gameMemory->createNewMemory(copyNumGrid, copyColorGrid, copyNoteColor);
			

			// Tests if the board is completed, and if so, transition to the game over screen
			if (testIfBoardComplete())
				currentGameState = OVER_SCREEN;

			// Drawing the new frame
			gameWindow.clear();
			if (gameBoard != nullptr)
				drawBoard();
			if (highlighter != nullptr)
				drawHighlighter();
			if (gameBoard != nullptr)
				drawMainBoardNumbers();
			if (notepad != nullptr) {
				drawNotesOnBoard();
				drawNotepad();
			}
			drawGameScreenOverlays();
			if (isPauseScreenOpen == true)
				drawPauseScreen();
			gameWindow.display();
		}
	}
}

// handleGameEvents function
void Game::handleGameEvents()
{
	// For keyboard input events from the player on the main game screen
	sf::Event event;
	while (gameWindow.pollEvent(event) && currentGameState == GAME_SCREEN)
	{
		
		// For the closing of the game screen
		if (event.type == sf::Event::Closed)
			currentGameState = EXIT_SCREEN;
		
		// For the pause screen
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			isPauseScreenOpen = !isPauseScreenOpen;

		if (isPauseScreenOpen == false)
		{
			// For the moving of the highligter
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && highlighter != nullptr)
			{
				highlighter->moveUp();
				if (!highlighter->isMoveValid())
					highlighter->moveDown();
			}
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && highlighter != nullptr)
			{
				highlighter->moveDown();
				if (!highlighter->isMoveValid())
					highlighter->moveUp();
			}
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) && highlighter != nullptr)
			{
				highlighter->moveLeft();
				if (!highlighter->isMoveValid())
					highlighter->moveRight();
			}
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) && highlighter != nullptr)
			{
				highlighter->moveRight();
				if (!highlighter->isMoveValid())
					highlighter->moveLeft();
			}

			// For the toggle of the notepad
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
					isNotepadOpen = !isNotepadOpen;
			}

			// For the providing of a hint
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H && hintCount > 0)
			{
				gameBoard->hint();
				hintCount--;
			}

			// For the undoing of the last move
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::U)
			{
				int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
				char copyColorGrid[BOARD_SIZE][BOARD_SIZE];
				char copyNoteColor[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
				gameBoard->getNumberGrid(copyNumGrid);
				gameBoard->getColorGrid(copyColorGrid);
				notepad->getAllColorCodes(copyNoteColor);

				gameMemory->deleteMemory(copyNumGrid, copyColorGrid, copyNoteColor);
				gameBoard->setNumberGrid(copyNumGrid);
				gameBoard->setColorGrid(copyColorGrid);
				notepad->setWholeNotepadColorCode(copyNoteColor);
			}

			// Completely resets the current board
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && highlighter != nullptr)
			{
				gameBoard->restartCurrentNumberGrid();
				notepad->resetWholeNotepadColorCode();
			}

			// Handling two different user input event cases based on if the notepad is open or not 
			if (isNotepadOpen)
				handleNoteEvents(event);
			else
				handleBoardEvents(event);
		}

		// For returning to the main menu (title screen)
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M)
		{
			delete gameBoard;
			gameBoard = nullptr;
			delete highlighter;
			highlighter = nullptr;
			delete notepad;
			notepad = nullptr;
			delete gameMemory;
			gameMemory = nullptr;
			currentGameState = TITLE_SCREEN;
		}
	}
}

// processOverScreen function
void Game::processOverScreen()
{
	finalTime = getPlayTime();
	// While the game window is open and on the game screen
	while (gameWindow.isOpen() && currentGameState == OVER_SCREEN)
	{
		// Handles user input events
		handleOverEvents();

		// Drawing the new frame
		gameWindow.clear();
		if (gameBoard != nullptr)
			drawBoard();
		if (gameBoard != nullptr)
			drawMainBoardNumbers();
		drawGameOverOverlays();
		if (isPauseScreenOpen == true)
			drawPauseScreen();
		gameWindow.display();
	}
}

// handleOverEvents function
void Game::handleOverEvents()
{
	// For keyboard input events from the player
	sf::Event event;
	while (gameWindow.pollEvent(event) && currentGameState == OVER_SCREEN)
	{

		// For the closing of the game screen
		if (event.type == sf::Event::Closed)
			currentGameState = EXIT_SCREEN;

		// For the pause screen
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			isPauseScreenOpen = !isPauseScreenOpen;

		if (isPauseScreenOpen == false) {
			// For returning to the main menu (title screen)
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M)
			{
				currentGameState = TITLE_SCREEN;
				delete gameBoard;
				gameBoard = nullptr;
				delete highlighter;
				highlighter = nullptr;
				delete notepad;
				notepad = nullptr;
				delete gameMemory;
				gameMemory = nullptr;
			}
		}
	}
}

// testIfPieceFit function
void Game::testIfPiecesFit()
{
	// Variables for copies of the gameBoards number and color code grids
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializing the grid copies to the current gameBoard
	gameBoard->getNumberGrid(copyNumGrid);
	gameBoard->getColorGrid(copyColorGrid);

	// Sets the color code of the numbers based on if it fits or not (r = doesn't fit (red), b = does fit (blue))
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (copyNumGrid[i][j] != 0 && (copyColorGrid[i][j] == 'b' || copyColorGrid[i][j] == 'r'))
			{
				if (!gameBoard->testIfNewPieceFits(copyNumGrid, copyNumGrid[i][j], i, j))
					copyColorGrid[i][j] = 'r';
				else 
					copyColorGrid[i][j] = 'b';
			}
		}
	}

	// Updates the current gameBoard color grid
	gameBoard->setColorGrid(copyColorGrid);
}

// testIfNotesFIt function
void Game::testIfNotesFit()
{
	// Variable for the copy of the notepad color code grid
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializes the copy of the notepad color code grid
	notepad->getAllColorCodes(copyColorGrid);
	gameBoard->getNumberGrid(copyNumGrid);

	// Sets the color codes to 'r' (red = doesn't fit) or 'g' (green = does fit) depending on if the note fits or not at the current grid index
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			for (int k = 0; k < BOARD_SIZE; k++)
			{
				if (copyColorGrid[i][j][k] != 'b') // If the color code isn't 'b' (b = blank) update to 'r' or 'g'
				{
					if (!gameBoard->testIfNewPieceFits(copyNumGrid, k + 1, i, j))
						copyColorGrid[i][j][k] = 'r';
					else
						copyColorGrid[i][j][k] = 'g';
				}
			}
		}
	}

	// Updates the current color code grid
	notepad->setWholeNotepadColorCode(copyColorGrid);
}

// testIfBoardComplete function
bool Game::testIfBoardComplete()
{
	// Variables for copies of the gameBoards number and color code grids
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializing the grid copies to the current gameBoard
	gameBoard->getNumberGrid(copyNumGrid);
	gameBoard->getColorGrid(copyColorGrid);

	// Checks if all numbers are placed and not color coded to 'r' (all pieces fit perfectly)
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (copyNumGrid[i][j] == 0 || copyColorGrid[i][j] == 'r')
				return false;
		}
	}
	return true;
}

// getPlayTime function
std::string Game::getPlayTime()
{
	std::string secondString;
	std::string minuteString;

	int second = static_cast<int>(gameClock.getElapsedTime().asSeconds()) % 60;
	int minute = static_cast<int>(gameClock.getElapsedTime().asSeconds()) / 60;

	if (second < 10)
		secondString = "0" + std::to_string(second);
	else if (second >= 10)
		secondString = std::to_string(second);

	if (minute < 10)
		minuteString = "0" + std::to_string(minute);
	else if (minute >= 10)
		minuteString = std::to_string(minute);

	return minuteString + ":" + secondString;
	
}

// handleBoardEvents function
void Game::handleBoardEvents(sf::Event event)
{
	// Variables for copies of each of the gameBoards number and color code grids
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializing the grid copies to the current gameBoard
	gameBoard->getNumberGrid(copyNumGrid);
	gameBoard->getColorGrid(copyColorGrid);

	// As long as the current highlighter position is on the location of a non-solid number then accept these user input events for placing new numbers on the board
	if (copyColorGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] != 'k' &&
		copyColorGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] != 'p')
	{
		for (int n = 1; n <= 9; ++n)
		{
			// For placing numbers onto the current highlighted tile of the board
			auto code = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + n - 1);
			auto numpad = static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad1 + n - 1);
			if (event.type == sf::Event::KeyPressed && (event.key.code == code || event.key.code == numpad) && highlighter != nullptr) 
			{
				if (copyNumGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] == n)
					copyNumGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] = 0;
				else
					copyNumGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] = n;

				gameBoard->setNumberGrid(copyNumGrid);
				notepad->resetCurrentPositionColorCodes((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS);
			}
		}

		// For deleting a number on the highlighted tile from the board
		if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::BackSpace || event.key.code == sf::Keyboard::Numpad0 || event.key.code == sf::Keyboard::Num0) && highlighter != nullptr)
		{
			copyNumGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] = 0;
			gameBoard->setNumberGrid(copyNumGrid);
		}
	}
}

// handleNoteEvents function
void Game::handleNoteEvents(sf::Event event)
{
	// Variable for the copy of the gamebaord number grid
	int copyNumberGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializes the copy of the color code grid
	gameBoard->getNumberGrid(copyNumberGrid);

	// As long as the current highlighter position is on the location of an empty space then accept these user input events for placing new notes on the board
	if (copyNumberGrid[(highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS][(highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS] == 0)
	{
		for (int n = 1; n <= 9; ++n)
		{
			// For placing notes onto the current highlighted tile of the board
			auto code = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + n - 1);
			auto numpad = static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad1 + n - 1);
			if (event.type == sf::Event::KeyPressed && (event.key.code == code || event.key.code == numpad) && highlighter != nullptr)
			{
				if (notepad->getSpecificColorCode((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS, n) == 'b')
					notepad->setSpecificColorCode((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS, n, 'g');
				else
					notepad->setSpecificColorCode((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS, n, 'b');
			}
		}
	}

	// For deleting the notes on the highlighted tile from the board
	if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::BackSpace || event.key.code == sf::Keyboard::Numpad0 || event.key.code == sf::Keyboard::Num0) && highlighter != nullptr)
		notepad->resetCurrentPositionColorCodes((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS);
}

// drawBoard function
void Game::drawBoard()
{
	// Creating the pixels to draw the gameBoard with
	sf::RectangleShape pixel;
	pixel.setFillColor(sf::Color(255, 255, 255));
	pixel.setOutlineColor(sf::Color(0, 0, 0));
	pixel.setOutlineThickness(1);
	pixel.setSize(sf::Vector2f(BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS));

	// Creating the gameBoard
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
 			if (((i >= 3 && i <= 5) && (j >= 0 && j <= 2)) || ((i >= 3 && i <= 5) && (j >= 6 && j <= 8)) || ((i >= 0 && i <= 2) && (j >= 3 && j <= 5)) || ((i >= 6 && i <= 8) && (j >= 3 && j <= 5)))
				pixel.setFillColor(sf::Color(170, 170, 170));
			else
				pixel.setFillColor(sf::Color(255, 255, 255));

			pixel.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + (j * BLOCK_SIZE_PIXELS), LAYOUT_BOARD_TOP + (i * BLOCK_SIZE_PIXELS)));
			gameWindow.draw(pixel);
		}
	}
}

// drawHighlighter function
void Game::drawHighlighter()
{
	// Creating a different colored pixel block for the highlighter, based on if the notepad is active or not
	sf::RectangleShape pixel;
	if (isNotepadOpen == true)
		pixel.setFillColor(sf::Color(0, 255, 255)); // Set's the highlighter to teal
	else
		pixel.setFillColor(sf::Color(255, 255, 0)); // Set's the highlighter to yellow
	pixel.setOutlineColor(sf::Color(0, 0, 0));
	pixel.setOutlineThickness(1);
	pixel.setSize(sf::Vector2f(BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS));
	pixel.setPosition(sf::Vector2f(highlighter->getLocation().column, highlighter->getLocation().row));
	gameWindow.draw(pixel);
}

// drawNumbers function
void Game::drawMainBoardNumbers()
{
	// Variables for the sudoku board numbers and color code copies
	int copyNumGrid[BOARD_SIZE][BOARD_SIZE];
	char copyColorGrid[BOARD_SIZE][BOARD_SIZE];

	// Initializes the copies of the number and color grids of the current gameBoard
	gameBoard->getNumberGrid(copyNumGrid);
	gameBoard->getColorGrid(copyColorGrid);

	// Creating the visible numbers on the board
	sf::Text visibleNumber;
	visibleNumber.setFont(gameFont);
	visibleNumber.setCharacterSize(70);

	// (0 == not visible on the board)
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (copyNumGrid[i][j] != 0) {
				if (copyColorGrid[i][j] == 'b') // 'b' = blue
					visibleNumber.setFillColor(sf::Color(0, 0, 255));
				else if (copyColorGrid[i][j] == 'k') // 'k' = black
					visibleNumber.setFillColor(sf::Color(0, 0, 0));
				else if (copyColorGrid[i][j] == 'r') // 'r' = red
					visibleNumber.setFillColor(sf::Color(255, 0, 0));
				else if (copyColorGrid[i][j] == 'p') // 'p' = purple
					visibleNumber.setFillColor(sf::Color(155, 0, 155));
				visibleNumber.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + 16 + (j * BLOCK_SIZE_PIXELS), LAYOUT_BOARD_TOP - 18 + (i * BLOCK_SIZE_PIXELS)));
				visibleNumber.setString(std::to_string(copyNumGrid[i][j]));
				gameWindow.draw(visibleNumber);
			}
		}
	}
}

// drawNotepad function
void Game::drawNotepad()
{
	// Arrays for the copies of the numbers and color codes in the notepad
	int copyNotepad[BOARD_SIZE];
	char copyNotepadColorCodes[BOARD_SIZE];

	// Initializing the copy arrays
	notepad->getCurrentPositionNotes((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS, copyNotepad);
	notepad->getCurrentPositionColorCodes((highlighter->getLocation().row - LAYOUT_BOARD_TOP) / BLOCK_SIZE_PIXELS, (highlighter->getLocation().column - LAYOUT_BOARD_LEFT) / BLOCK_SIZE_PIXELS, copyNotepadColorCodes);

	// Creating the pixels and text to draw with
	sf::Text notedNumbers;
	notedNumbers.setFont(gameFont);
	notedNumbers.setCharacterSize(100);
	notedNumbers.setFillColor(sf::Color(0, 0, 0));

	sf::RectangleShape pixel;
	pixel.setFillColor(sf::Color(255, 255, 255));
	pixel.setOutlineColor(sf::Color(0, 0, 0));
	pixel.setOutlineThickness(1);
	pixel.setSize(sf::Vector2f(BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS));

	// Draws a single row of (BOARD_SIZE) notes (visible/invisible) below the main game board, representing the notes for the currently highlighted location on the main board
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		pixel.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + (i * BLOCK_SIZE_PIXELS), LAYOUT_BOARD_TOP + ((BOARD_SIZE + 1) * BLOCK_SIZE_PIXELS)));
		notedNumbers.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + 8 + (i * BLOCK_SIZE_PIXELS), LAYOUT_BOARD_TOP - 40 + ((BOARD_SIZE + 1) * BLOCK_SIZE_PIXELS)));
		gameWindow.draw(pixel);
		if (copyNotepadColorCodes[i] != 'b')
		{
			if (copyNotepadColorCodes[i] == 'g')
				notedNumbers.setFillColor(sf::Color(0, 255, 0));
			else if (copyNotepadColorCodes[i] == 'r')
				notedNumbers.setFillColor(sf::Color(255, 0, 0));
			notedNumbers.setString(std::to_string(copyNotepad[i]));
			gameWindow.draw(notedNumbers);
		}
	}
}

// drawNotesOnBoard function
void Game::drawNotesOnBoard()
{
	// Variables for the creation of mimi note grids on each individual spot on the game board
	char copyNotepadColorCodes[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	int copyBoardNums[BOARD_SIZE][BOARD_SIZE];

	// Grabs copy grids of the color codes and numbers for the notes at the highlighters position as well as a copy of the current main boards grid
	notepad->getAllColorCodes(copyNotepadColorCodes);
	gameBoard->getNumberGrid(copyBoardNums);

	// Creating the mini notes to go onto the game board
	sf::Text notesOnBoard;
	notesOnBoard.setFont(gameFont);
	notesOnBoard.setCharacterSize(30);
	notesOnBoard.setFillColor(sf::Color(0, 0, 0));

	// Depending on the color code of the note on the board, set the color to the corresponding number on the board
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			for (int k = 0; k < BOARD_SIZE; k++)
			{
				if (copyNotepadColorCodes[i][j][k] == 'g') // 'g' = green
					notesOnBoard.setFillColor(sf::Color(0, 255, 0));
				else if (copyNotepadColorCodes[i][j][k] == 'r') // 'r' = red
					notesOnBoard.setFillColor(sf::Color(255, 0, 0));

				// Drawing each note on the window/game board depending on where it's at on the notepad grids
				if (k >= 0 && k < BOARD_SIZE / 3 && copyNotepadColorCodes[i][j][k] != 'b' && copyBoardNums[i][j] == 0)
				{
					notesOnBoard.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + (j * BLOCK_SIZE_PIXELS) + (k * 20) + 6, LAYOUT_BOARD_TOP + (i * BLOCK_SIZE_PIXELS) - 7));
					notesOnBoard.setString(std::to_string(k + 1));
					gameWindow.draw(notesOnBoard);
				}
				else if (k >= BOARD_SIZE / 3 && k < BOARD_SIZE / 3 * 2 && copyNotepadColorCodes[i][j][k] != 'b' && copyBoardNums[i][j] == 0)
				{
					notesOnBoard.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + (j * BLOCK_SIZE_PIXELS) + (k * 20) -56, LAYOUT_BOARD_TOP + (i * BLOCK_SIZE_PIXELS) + 17));
					notesOnBoard.setString(std::to_string(k + 1));
					gameWindow.draw(notesOnBoard);

				}
				else if (k >= BOARD_SIZE / 3 * 2 && k < BOARD_SIZE && copyNotepadColorCodes[i][j][k] != 'b' && copyBoardNums[i][j] == 0)
				{
					notesOnBoard.setPosition(sf::Vector2f(LAYOUT_BOARD_LEFT + (j * BLOCK_SIZE_PIXELS) + (k * 20) - 116, LAYOUT_BOARD_TOP + (i * BLOCK_SIZE_PIXELS) + 39));
					notesOnBoard.setString(std::to_string(k + 1));
					gameWindow.draw(notesOnBoard);

				}
			}
		}
	}
}

// drawTitleScreenOverlays function
void Game::drawTitleScreenOverlays()
{
	// For the title on the title screen
	sf::Text sudokuTitle;
	sudokuTitle.setFont(gameFont);
	sudokuTitle.setCharacterSize(200);
	sudokuTitle.setString("MYSUDOKU");
	sudokuTitle.setPosition(sf::Vector2f(80, -15));
	sudokuTitle.setFillColor(sf::Color(0, 200, 255));

	// For the pause button description
	sf::Text playAndPause;
	playAndPause.setFont(gameFont);
	playAndPause.setCharacterSize(80);
	playAndPause.setString("Press 'Esc' To See \nInstructions Menu!");
	playAndPause.setPosition(sf::Vector2f(100, 220));
	playAndPause.setFillColor(sf::Color(190, 96, 255));

	// For the space button description
	sf::Text spaceToPlay;
	spaceToPlay.setFont(gameFont);
	spaceToPlay.setCharacterSize(80);
	spaceToPlay.setString("Press 'Space' To Play!");
	spaceToPlay.setPosition(sf::Vector2f(100, 380));
	spaceToPlay.setFillColor(sf::Color(190, 96, 255));

	// For the change difficulty description
	sf::Text tabDifficulty;
	tabDifficulty.setFont(gameFont);
	tabDifficulty.setCharacterSize(80);
	tabDifficulty.setString("Press 'Tab' to \nchange difficulty:");
	tabDifficulty.setPosition(sf::Vector2f(100, 490));
	tabDifficulty.setFillColor(sf::Color(190, 96, 255));

	// For the expert load warning
	sf::Text secondToLoad;
	secondToLoad.setFont(gameFont);
	secondToLoad.setCharacterSize(60);
	secondToLoad.setString("May Take A \nSecond To Load...");
	secondToLoad.setPosition(sf::Vector2f(100, 800));
	secondToLoad.setFillColor(sf::Color(140, 140, 255));

	// For the current difficulty display
	sf::Text difficultyDisplay;
	difficultyDisplay.setFont(gameFont);
	difficultyDisplay.setCharacterSize(100);
	difficultyDisplay.setPosition(sf::Vector2f(100, 660));
	if (currentDifficulty == EASY)
	{
		difficultyDisplay.setString("EASY");
		difficultyDisplay.setFillColor(sf::Color(0, 255, 0));
	}
	else if (currentDifficulty == NORMAL)
	{
		difficultyDisplay.setString("NORMAL");
		difficultyDisplay.setFillColor(sf::Color(255, 255, 0));
	}
	else if (currentDifficulty == HARD)
	{
		difficultyDisplay.setString("HARD");
		difficultyDisplay.setFillColor(sf::Color(255, 128, 0));
	}
	else if (currentDifficulty == EXPERT)
	{
		difficultyDisplay.setString("EXPERT");
		difficultyDisplay.setFillColor(sf::Color(255, 60, 60));
	}
	else if (currentDifficulty == IMPOSSIBLE)
	{
		difficultyDisplay.setString("IMPOSSIBLE");
		difficultyDisplay.setFillColor(sf::Color(200, 0, 0));
	}
	else if (currentDifficulty == BEGINNER)
	{
		difficultyDisplay.setString("BEGINNER");
		difficultyDisplay.setFillColor(sf::Color(160, 255, 160));
	}

	// Drawing all the current text to the current window
	gameWindow.draw(sudokuTitle);
	gameWindow.draw(playAndPause);
	gameWindow.draw(spaceToPlay);
	gameWindow.draw(tabDifficulty);
	gameWindow.draw(difficultyDisplay);
	if (currentDifficulty == EXPERT || currentDifficulty == IMPOSSIBLE)
		gameWindow.draw(secondToLoad);
}

// drawGameScreenOverlays function
void Game::drawGameScreenOverlays()
{
	// For the indicators for if the notepad is active or if the game board is active
	sf::Text notepadActiveOrNot;
	notepadActiveOrNot.setFont(gameFont);
	notepadActiveOrNot.setCharacterSize(75);

	if (isNotepadOpen == true)
	{
		notepadActiveOrNot.setFillColor(sf::Color(0, 255, 255));
		notepadActiveOrNot.setString("NOTE TAKING:");
		notepadActiveOrNot.setPosition(sf::Vector2f(265, -5));
	}
	else
	{
		notepadActiveOrNot.setFillColor(sf::Color(255, 255, 0));
		notepadActiveOrNot.setString("NUMBER PLACING:");
		notepadActiveOrNot.setPosition(sf::Vector2f(235, -5));
	}

	// For the hint text
	sf::Text hintText;
	hintText.setFont(gameFont);
	hintText.setCharacterSize(60);
	hintText.setFillColor(sf::Color(155, 0, 155));
	hintText.setString("HINTS: " + std::to_string(hintCount));
	hintText.setPosition(sf::Vector2f(170, 915));

	// For the game clock
	sf::Text clockText;
	clockText.setFont(gameFont);
	clockText.setCharacterSize(60);
	clockText.setFillColor(sf::Color(155, 0, 155));
	clockText.setString(getPlayTime());
	clockText.setPosition(sf::Vector2f(700, 915));

	// For that notes section indicator
	sf::Text currentLocationNotes;
	currentLocationNotes.setFont(gameFont);
	currentLocationNotes.setCharacterSize(60);
	currentLocationNotes.setFillColor(sf::Color(255, 125, 0));
	currentLocationNotes.setString("CURRENT POSITION NOTES:");
	currentLocationNotes.setPosition(sf::Vector2f(180, 765));

	// Drawing all the current text to the current window
	gameWindow.draw(notepadActiveOrNot);
	gameWindow.draw(currentLocationNotes);
	gameWindow.draw(hintText);
	gameWindow.draw(clockText);
}

void Game::drawGameOverOverlays()
{
	// For the board text
	sf::Text boardText;
	boardText.setFont(gameFont);
	boardText.setCharacterSize(100);
	boardText.setFillColor(sf::Color(0, 255, 44));
	boardText.setString("Board Complete!");
	boardText.setPosition(sf::Vector2f(140, -20));
	
	// For the final time text
	sf::Text finalTimeText;
	finalTimeText.setFont(gameFont);
	finalTimeText.setCharacterSize(60);
	finalTimeText.setFillColor(sf::Color(155, 0, 155));
	finalTimeText.setString("Final Time: " + finalTime);
	finalTimeText.setPosition(sf::Vector2f(280, 770));
	
	// For the hints used text
	sf::Text finalHintsText;
	finalHintsText.setFont(gameFont);
	finalHintsText.setCharacterSize(60);
	finalHintsText.setFillColor(sf::Color(155, 0, 155));
	finalHintsText.setString("Hints Used: " + std::to_string(3 - hintCount) + "/3");
	finalHintsText.setPosition(sf::Vector2f(290, 830));

	// For the space button description
	sf::Text mToPlay;
	mToPlay.setFont(gameFont);
	mToPlay.setCharacterSize(60);
	mToPlay.setString("Press 'M' To Play Again!");
	mToPlay.setPosition(sf::Vector2f(190, 890));
	mToPlay.setFillColor(sf::Color(0, 255, 44));

	// Drawing all the current text to the current window
	gameWindow.draw(boardText);
	gameWindow.draw(finalTimeText);
	gameWindow.draw(finalHintsText);
	gameWindow.draw(mToPlay);
}

void Game::drawPauseScreen()
{
	// For the pause block
	sf::RectangleShape pauseBlock;
	pauseBlock.setSize(sf::Vector2f(800, 800));
	pauseBlock.setPosition(sf::Vector2f(100, 100));
	pauseBlock.setOutlineColor(sf::Color(255, 255, 255));
	pauseBlock.setOutlineThickness(1);
	pauseBlock.setFillColor(sf::Color(0, 0, 0));

	// For the game paused text
	sf::Text gamePaused;
	gamePaused.setString("Game Paused");
	gamePaused.setFont(gameFont);
	gamePaused.setCharacterSize(120);
	gamePaused.setPosition(170, 110);

	// For the controls text
	sf::Text controls;
	controls.setString("\'H\' = Hint \n'M\' = Main Menu \n\'Esc\' = Controls \n\'U\' = Undo Action \n\'R\' = Reset Board \n\'Backspace\' = Clear Tile \n\'Space\' = Toggle Notepad \n\'Arrows\' = Move Highlighter");
	controls.setFont(gameFont);
	controls.setCharacterSize(50);
	controls.setPosition(200, 260);

	// For the goal of the game description
	sf::Text goal;
	goal.setString("Goal of Sudoku: \nEach Row, Column, and 3x3 \nsections may only have the \nnumbers 1-9, NO DUPLICATES");
	goal.setFont(gameFont);
	goal.setCharacterSize(50);
	goal.setPosition(200, 640);

	// Drawing all the current text to the current window
	gameWindow.draw(pauseBlock);
	gameWindow.draw(gamePaused);
	gameWindow.draw(controls);
	gameWindow.draw(goal);
}