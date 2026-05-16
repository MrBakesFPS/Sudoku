/* ===================================================================================================================
@Author:		Tyson Koopman-Baker
@Date:			8/22/2025
@File:			Board.cpp
@Version:		1.0
@IDE:			Microsoft Visual Studios 2022 - Community
@Description:	This is the main .cpp file for the Board class, used as the main game board for the sudoku game.
=================================================================================================================== */

// Include files
#include "Board.h"
#include <vector>
#include <random>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include "Constants.h"

// Globally seed random number generator
static std::mt19937 rng(std::random_device{}());

// Default Board constructor
Board::Board()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = 0;
			solvedBoard[i][j] = 0;
			colorGrid[i][j] = 'b';
		}
	}
}

// ChatGPT V1 initializeNewBoard function
bool Board::initializeNewBoard()
{
	auto pattern = [](int r, int c) {
		// Base Sudoku pattern (valid solved grid)
		return (r * 3 + r / 3 + c) % 9;
		};

	// Helper to make a shuffled list
	auto shuffled = [&](std::vector<int> v) {
		std::shuffle(v.begin(), v.end(), rng);
		return v;
		};

	// Row/col indices via bands/stacks shuffling
	std::vector<int> base = { 0,1,2 };

	auto rows = [&]() {
		std::vector<int> out;
		auto bands = shuffled(base);
		for (int b : bands) {
			auto inner = shuffled(base);
			for (int r : inner) out.push_back(b * 3 + r);
		}
		return out;
		}();

	auto cols = [&]() {
		std::vector<int> out;
		auto stacks = shuffled(base);
		for (int s : stacks) {
			auto inner = shuffled(base);
			for (int c : inner) out.push_back(s * 3 + c);
		}
		return out;
		}();

	// Digit permutation 1..9
	std::vector<int> nums = { 1,2,3,4,5,6,7,8,9 };
	nums = shuffled(nums);

	// Build solved grid
	for (int r = 0; r < BOARD_SIZE; ++r) {
		for (int c = 0; c < BOARD_SIZE; ++c) {
			numberGrid[r][c] = nums[pattern(rows[r], cols[c])];
			colorGrid[r][c] = 'b';
		}
	}

	return true;
}

// ChatGPT V1 createSolidPieces function
void Board::createSolidPieces(Difficulty difficulty)
{
	int maxEmptySpaces = 0;
	if (difficulty == BEGINNER) maxEmptySpaces = 24;
	else if (difficulty == EASY) maxEmptySpaces = 32;
	else if (difficulty == NORMAL) maxEmptySpaces = 40;
	else if (difficulty == HARD) maxEmptySpaces = 48;
	else if (difficulty == EXPERT) maxEmptySpaces = 56;
	else if (difficulty == IMPOSSIBLE) maxEmptySpaces = 64;

	if (maxEmptySpaces <= 56)
	{
		initializeNewBoard();

		// Build list of all cells and shuffle once
		std::vector<std::pair<int, int>> cells;
		cells.reserve(81);
		for (int r = 0; r < 9; ++r)
			for (int c = 0; c < 9; ++c)
				cells.push_back({ r,c });

		std::shuffle(cells.begin(), cells.end(), rng);

		int currentEmptySpaces = 0;

		for (const auto& cell : cells) {
			int r = cell.first;
			int c = cell.second;
			if (currentEmptySpaces >= maxEmptySpaces)
				break;

			int backup = numberGrid[r][c];
			if (backup == 0)
				continue;

			numberGrid[r][c] = 0;

			if (isCurrentBoardUnique())
				currentEmptySpaces++;
			else
				numberGrid[r][c] = backup;
		}
	}
	else
	{
		seventeenClueBoard();
		mixBoard();
	}

	// Mark remaining numbers as clues
	for (int r = 0; r < 9; ++r)
	{
		for (int c = 0; c < 9; ++c)
		{
			if (numberGrid[r][c] != 0)
				colorGrid[r][c] = 'k';
			else
				colorGrid[r][c] = 'b';
		}
	}

	getNumberGrid(solvedBoard);
	createSolvedBoard(solvedBoard);
}

// isCurrentBoardUnique function
bool Board::isCurrentBoardUnique()
{
	// For the copy of the board thats being tested to see if it is a uniquely solvable board
	int copyNumberGrid[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(copyNumberGrid);

	return uniqueBoardSolves(copyNumberGrid) == 1;
}

// uniqueBoardSolves recursive function
int Board::uniqueBoardSolves(int boardBeingSolved[BOARD_SIZE][BOARD_SIZE])
{
	int bestR = -1, bestC = -1;
	int bestCount = 10;
	int opts[9]{};
	int optCount = 0;

	for (int r = 0; r < BOARD_SIZE; ++r)
	{
		for (int c = 0; c < BOARD_SIZE; ++c)
		{
			if (boardBeingSolved[r][c] != 0)
				continue;

			int localOpts[9]{};
			int cnt = 0;
			for (int n = 1; n <= 9; ++n)
				if (testIfNewPieceFits(boardBeingSolved, n, r, c))
					localOpts[cnt++] = n;

			if (cnt == 0)
				return 0;

			if (cnt < bestCount)
			{
				bestCount = cnt;
				bestR = r;
				bestC = c;
				optCount = cnt;
				for (int i = 0; i < cnt; ++i)
					opts[i] = localOpts[i]; // save best candidates
				if (bestCount == 1)
					break;
			}
		}
		if (bestCount == 1)
			break;
	}

	if (bestR == -1)
		return 1;

	int count = 0;
	for (int i = 0; i < optCount; ++i)
	{
		boardBeingSolved[bestR][bestC] = opts[i];
		count += uniqueBoardSolves(boardBeingSolved);
		if (count > 1) { // early exit once not unique
			boardBeingSolved[bestR][bestC] = 0;
			return count;
		}
		boardBeingSolved[bestR][bestC] = 0;
	}

	return count;
}

// createSolvedBoard function
bool Board::createSolvedBoard(int solvedGrid[BOARD_SIZE][BOARD_SIZE])
{
	// Find the first empty cell using MRV (same heuristic as uniqueBoardSolves)
	int bestR = -1, bestC = -1;
	int bestCount = 10;

	for (int r = 0; r < BOARD_SIZE; ++r)
	{
		for (int c = 0; c < BOARD_SIZE; ++c)
		{
			if (solvedGrid[r][c] != 0)
				continue;

			int cnt = 0;
			for (int n = 1; n <= 9; ++n)
				if (testIfNewPieceFits(solvedGrid, n, r, c))
					cnt++;

			if (cnt == 0) return false; // Dead end
			if (cnt < bestCount)
			{
				bestCount = cnt;
				bestR = r; bestC = c;
				if (bestCount == 1) break;
			}
		}
		if (bestCount == 1) break;
	}

	if (bestR == -1) return true; // No empty cells — board is already solved

	// Try each valid candidate for the chosen cell
	for (int n = 1; n <= 9; ++n)
	{
		if (testIfNewPieceFits(solvedGrid, n, bestR, bestC))
		{
			solvedGrid[bestR][bestC] = n;
			if (createSolvedBoard(solvedGrid)) // Note: this recurses using the updated grid
				return true;
			solvedGrid[bestR][bestC] = 0; // Backtrack
		}
	}

	return false; // No valid number found — trigger backtrack
}

// hint function
void Board::hint()
{
	// Gather all empty cell coordinates
	std::vector<std::pair<int, int>> emptyCells;
	for (int row = 0; row < BOARD_SIZE; row++)
		for (int col = 0; col < BOARD_SIZE; col++)
			if (numberGrid[row][col] == 0)
				emptyCells.emplace_back(row, col);

	// Nothing to reveal
	if (emptyCells.empty())
		return;

	std::uniform_int_distribution<> distrib(0, static_cast<int>(emptyCells.size()) - 1);
	auto [row, col] = emptyCells[distrib(rng)];

	numberGrid[row][col] = solvedBoard[row][col];
	colorGrid[row][col] = 'p'; // Mark as a given clue, not a player entry
}

// setNumberGrid function
void Board::setNumberGrid(int gridIn[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = gridIn[i][j];
		}
	}
}

// setColorGrid function
void Board::setColorGrid(char gridIn[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			colorGrid[i][j] = gridIn[i][j];
		}
	}
}

// testIfNewPieceFits function
bool Board::testIfNewPieceFits(int numberGridIn[BOARD_SIZE][BOARD_SIZE], int newNum, int currRow, int currCol) const
{
	// Tests to see if the new number exists on the specified column already
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		if (i == currRow)
			continue;
		if (numberGridIn[i][currCol] == newNum)
			return false;
	}

	// Tests to see if the new number exists on the specified row already
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (i == currCol)
			continue;
		if (numberGridIn[currRow][i] == newNum)
			return false;
	}

	// Tests to see if the new numbers exist on the specified 3x3 already
	int startRow = (currRow / 3) * 3;
	int startCol = (currCol / 3) * 3;

	for (int i = startRow; i < startRow + 3; ++i)
	{
		for (int j = startCol; j < startCol + 3; ++j)
		{
			if (i == currRow && j == currCol)
				continue;
			if (numberGridIn[i][j] == newNum)
				return false;
		}
	}

	return true;
}

// getNumberGrid function
void Board::getNumberGrid(int gridOut[BOARD_SIZE][BOARD_SIZE]) const
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			gridOut[i][j] = numberGrid[i][j];
		}
	}
}

// getColorGrid function
void Board::getColorGrid(char gridOut[BOARD_SIZE][BOARD_SIZE]) const
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			gridOut[i][j] = colorGrid[i][j];
		}
	}
}

// restartCurrentNumberGrid function
void Board::restartCurrentNumberGrid()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (colorGrid[i][j] != 'k' && colorGrid[i][j] != 'p')
			{
				numberGrid[i][j] = 0;
				colorGrid[i][j] = 'b';
			}
		}
	}
}

// seventeenClueBoard function
void Board::seventeenClueBoard()
{
	std::uniform_int_distribution<> distrib(1, SEVENTEEN_CLUE_BOARD);
	int targetLine = distrib(rng);

	std::ifstream puzzleFile("17_clue_boards.txt");
	std::string lineContent;
	int currentLine = 0;
	int currentLinePos = 0;

	int puzzle[BOARD_SIZE][BOARD_SIZE]{};

	if (puzzleFile.is_open())
	{
		while (std::getline(puzzleFile, lineContent))
		{
			currentLine++;
			if (currentLine == targetLine)
			{
				for (int i = 0; i < BOARD_SIZE; i++)
				{
					for (int j = 0; j < BOARD_SIZE; j++)
					{
						if (lineContent.at(currentLinePos) == '.')
							puzzle[i][j] = 0;
						else
							puzzle[i][j] = lineContent.at(currentLinePos) - '0';

						currentLinePos++;
					}
				}
				break;
			}
		}
	}
	else
	{
		std::cout << "File could not open!" << std::endl;
		return;
	}

	// Build solved grid
	for (int r = 0; r < BOARD_SIZE; ++r)
	{
		for (int c = 0; c < BOARD_SIZE; ++c)
		{
			numberGrid[r][c] = puzzle[r][c];
			if (numberGrid[r][c] == 0)
				colorGrid[r][c] = 'b';
			else
				colorGrid[r][c] = 'k';
		}
	}
}

// mixBoard function
void Board::mixBoard()
{
	std::uniform_int_distribution<> distrib(1, 11);

	for (int i = 0; i < 100; i++)
	{
		switch (distrib(rng))
		{
		case 1:
		{
			swap1();
			break;
		}
		case 2:
		{
			swap2();
			break;
		}
		case 3:
		{
			swap3();
			break;
		}
		case 4:
		{
			swap4();
			break;
		}
		case 5:
		{
			swap5();
			break;
		}
		case 6:
		{
			swap6();
			break;
		}
		case 7:
		{
			swap7();
			break;
		}
		case 8:
		{
			swap8();
			break;
		}
		case 9:
		{
			swap9();
			break;
		}
		case 10:
		{
			swap10();
			break;
		}
		case 11:
		{
			swap11();
			break;
		}
		}
	}

}

// Top 3 with bottom 3
void Board::swap1()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE / 3; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = tempBoard[i + 6][j];
			numberGrid[i + 6][j] = tempBoard[i][j];
		}
	}
}

// Left 3 with right 3
void Board::swap2()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int j = 0; j < BOARD_SIZE / 3; j++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			numberGrid[i][j] = tempBoard[i][j + 6];
			numberGrid[i][j + 6] = tempBoard[i][j];
		}
	}
}

// Left 3 with Middle 3
void Board::swap3()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int j = 0; j < BOARD_SIZE / 3; j++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			numberGrid[i][j] = tempBoard[i][j + 3];
			numberGrid[i][j + 3] = tempBoard[i][j];
		}
	}
}

// Middle 3 with Right 3
void Board::swap4()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int j = 0; j < BOARD_SIZE / 3; j++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			numberGrid[i][j + 3] = tempBoard[i][j + 6];
			numberGrid[i][j + 6] = tempBoard[i][j + 3];
		}
	}
}

// Top 3 with middle 3
void Board::swap5()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE / 3; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = tempBoard[i + 3][j];
			numberGrid[i + 3][j] = tempBoard[i][j];
		}
	}
}

// Middle 3 with bottom 3
void Board::swap6()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE / 3; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i + 3][j] = tempBoard[i + 6][j];
			numberGrid[i + 6][j] = tempBoard[i + 3][j];
		}
	}
}

// Mirror Top to Bottom
void Board::swap7()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = tempBoard[BOARD_SIZE - i - 1][j];
		}
	}
}

// Mirror left to right
void Board::swap8()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[i][j] = tempBoard[i][BOARD_SIZE - j - 1];
		}
	}
}

// Rotate left
void Board::swap9()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[BOARD_SIZE - 1 - j][i] = tempBoard[i][j];
		}
	}
}

// Rotate right
void Board::swap10()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			numberGrid[j][BOARD_SIZE - 1 - i] = tempBoard[i][j];
		}
	}
}

// Swap inner row/col within a section of the board
void Board::swap11()
{
	int tempBoard[BOARD_SIZE][BOARD_SIZE];
	getNumberGrid(tempBoard);
	
	std::uniform_int_distribution<> distrib(0, 1);
	int random_num = distrib(rng);

	if (random_num == 0)
	{
		std::uniform_int_distribution<> distrib(0, 2);
		int section_number_row = distrib(rng);

		int row_num_one = 0;
		int row_num_two = 0;

		while (row_num_one == row_num_two)
		{
			row_num_one = distrib(rng);
			row_num_two = distrib(rng);
		}

		row_num_one += section_number_row * 3;
		row_num_two += section_number_row * 3;

		for (int i = 0; i < BOARD_SIZE; i++)
		{
			numberGrid[row_num_one][i] = tempBoard[row_num_two][i];
			numberGrid[row_num_two][i] = tempBoard[row_num_one][i];
		}
	}
	else
	{
		std::uniform_int_distribution<> distrib(0, 2);
		int section_number_col = distrib(rng);

		int col_num_one = 0;
		int col_num_two = 0;

		while (col_num_one == col_num_two)
		{
			col_num_one = distrib(rng);
			col_num_two = distrib(rng);
		}

		col_num_one += section_number_col * 3;
		col_num_two += section_number_col * 3;

		for (int i = 0; i < BOARD_SIZE; i++)
		{
			numberGrid[i][col_num_one] = tempBoard[i][col_num_two];
			numberGrid[i][col_num_two] = tempBoard[i][col_num_one];
		}
	}

}