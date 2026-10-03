#include "Board.h"
#include "GameMemory.h"
#include "Notepad.h"
#include "PlayTimer.h"
#include "WindowLayout.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct State {
    int numbers[BOARD_SIZE][BOARD_SIZE]{};
    char colors[BOARD_SIZE][BOARD_SIZE]{};
    char notes[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE]{};
};

State capture(const Board& board, const Notepad& notes) {
    State state;
    board.getNumberGrid(state.numbers);
    board.getColorGrid(state.colors);
    notes.getAllColorCodes(state.notes);
    return state;
}
void remember(GameMemory& memory, State state) {
    memory.createNewMemory(state.numbers, state.colors, state.notes);
}
void undo(GameMemory& memory, Board& board, Notepad& notes) {
    State state = capture(board, notes);
    memory.deleteMemory(state.numbers, state.colors, state.notes);
    board.setNumberGrid(state.numbers);
    board.setColorGrid(state.colors);
    notes.setWholeNotepadColorCode(state.notes);
}
bool changed(GameMemory& memory, State state) {
    return memory.testIfChangesMade(state.numbers, state.colors, state.notes);
}
int hintCount(const State& state) {
    int count = 0;
    for (int row = 0; row < BOARD_SIZE; ++row)
        for (int col = 0; col < BOARD_SIZE; ++col)
            count += state.colors[row][col] == 'p';
    return count;
}
void preserve(GameMemory& memory, const State& state) {
    memory.preserveHints(state.numbers, state.colors);
}

void testPauseTimer() {
    using namespace std::chrono;
    const PlayTimer::TimePoint zero{};
    PlayTimer timer;
    require(timer.elapsed(zero + hours(1)).count() == 0, "unstarted timer is stopped");
    timer.restart(zero);
    require(timer.elapsed(zero + milliseconds(2500)).count() == 2, "timer counts active seconds");
    timer.pause(zero + milliseconds(2500));
    timer.pause(zero + seconds(9));
    require(timer.elapsed(zero + hours(1)).count() == 2, "paused time must not accumulate");
    timer.resume(zero + seconds(12));
    timer.resume(zero + seconds(15));
    require(timer.elapsed(zero + milliseconds(13500)).count() == 4, "resume preserves fractional active time");
    timer.pause(zero + seconds(17));
    timer.resume(zero + seconds(60));
    require(timer.elapsed(zero + seconds(63)).count() == 10, "multiple pauses exclude every paused interval");
    timer.pause(zero + seconds(63));
    require(timer.elapsed(zero + hours(2)).count() == 10, "completed game time stays frozen");
    timer.restart(zero + hours(3));
    require(timer.elapsed(zero + hours(3) + seconds(1)).count() == 1, "new game resets paused clock");
}

void testWindowLayout() {
    const auto small = WindowLayout::initialSize(1366, 768);
    require(small.width == 691 && small.height == 691, "768p desktop must fit with decorations margin");
    const auto tiny = WindowLayout::initialSize(640, 480);
    require(tiny.width == 432 && tiny.height == 432, "small desktop must not clip");
    const auto large = WindowLayout::initialSize(3840, 2160);
    require(large.width == 1000 && large.height == 1000, "large desktop retains default size");
    for (const auto size : {WindowLayout::Size{500, 500}, {800, 450}, {450, 800}, {1, 1}}) {
        const auto view = WindowLayout::viewport(size.width, size.height);
        require(view.left >= 0 && view.top >= 0 && view.width > 0 && view.height > 0, "viewport stays positive");
        require(view.left + view.width <= 1.00001f && view.top + view.height <= 1.00001f, "viewport stays inside window");
        require(std::fabs(view.width * size.width - view.height * size.height) < 0.01f, "board cells stay square");
    }
    const auto minimized = WindowLayout::viewport(0, 0);
    require(std::isfinite(minimized.width), "zero-size window must not divide by zero");
}

void testHintUndoAndReset() {
    Board board;
    Notepad notes;
    GameMemory memory;
    board.createSolidPieces(NORMAL);
    State initial = capture(board, notes);
    int solved[BOARD_SIZE][BOARD_SIZE];
    board.getNumberGrid(solved);
    require(board.createSolvedBoard(solved), "generated puzzle must solve");
    int hintRow = -1, hintCol = -1, moveRow = -1, moveCol = -1;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (initial.numbers[row][col] != 0) continue;
            if (hintRow == -1) { hintRow = row; hintCol = col; }
            else if (moveRow == -1) { moveRow = row; moveCol = col; }
        }
    }
    require(moveRow != -1, "puzzle supplies editable cells");
    // A note on the future hint must also disappear from older history.
    notes.setSpecificColorCode(hintRow, hintCol, 1, 'g');
    initial = capture(board, notes);
    remember(memory, initial);
    State firstMove = initial;
    firstMove.numbers[moveRow][moveCol] = solved[moveRow][moveCol];
    board.setNumberGrid(firstMove.numbers);
    remember(memory, capture(board, notes));
    State lastMove = capture(board, notes);
    for (int row = 0; row < BOARD_SIZE; ++row)
        for (int col = 0; col < BOARD_SIZE; ++col)
            if (row != hintRow || col != hintCol)
                lastMove.numbers[row][col] = solved[row][col];
    board.setNumberGrid(lastMove.numbers);
    remember(memory, capture(board, notes));
    require(board.hint(), "hint reveals remaining empty cell");
    State hinted = capture(board, notes);
    preserve(memory, hinted);
    notes.resetCurrentPositionColorCodes(hintRow, hintCol);
    require(!changed(memory, capture(board, notes)), "hint alone must not create an undoable move");
    require(!board.hint(), "full board has no consumable hint");
    undo(memory, board, notes);
    State undone = capture(board, notes);
    require(undone.numbers[hintRow][hintCol] == solved[hintRow][hintCol] && undone.colors[hintRow][hintCol] == 'p', "hint survives undo to pre-hint history");
    require(undone.numbers[moveRow][moveCol] == solved[moveRow][moveCol], "undo returns preceding player move");
    require(undone.notes[hintRow][hintCol][0] == 'b', "undo cannot resurrect notes under a hint");
    // Conflict validation may change red/blue and green/red colors after a hint.
    // Those are derived state, and must not push the undone move back on history.
    undone.colors[moveRow][moveCol] = 'r';
    board.setColorGrid(undone.colors);
    require(!changed(memory, capture(board, notes)), "derived recoloring must not trap undo");
    undo(memory, board, notes);
    undone = capture(board, notes);
    require(undone.numbers[moveRow][moveCol] == 0, "repeated undo reaches the original player state");
    for (int i = 0; i < 5; ++i) undo(memory, board, notes);
    board.restartCurrentNumberGrid();
    undone = capture(board, notes);
    require(undone.colors[hintRow][hintCol] == 'p' && undone.numbers[hintRow][hintCol] == solved[hintRow][hintCol], "reset retains hints");
    // Even a stale external snapshot cannot turn a consumed hint back into an editable cell.
    board.setNumberGrid(initial.numbers);
    board.setColorGrid(initial.colors);
    undone = capture(board, notes);
    require(undone.colors[hintRow][hintCol] == 'p' && undone.numbers[hintRow][hintCol] == solved[hintRow][hintCol], "persistent hint overlay protects stale snapshot restore");
    board.createSolidPieces(BEGINNER);
    require(hintCount(capture(board, notes)) == 0, "new puzzle must clear all previous hints");
}

void testMultipleHintsAndNotes() {
    Board board;
    Notepad notes;
    GameMemory memory;
    board.createSolidPieces(EASY);
    remember(memory, capture(board, notes));
    for (int i = 0; i < 3; ++i) {
        require(board.hint(), "each hint reveals a new cell");
        State state = capture(board, notes);
        preserve(memory, state);
        require(hintCount(state) == i + 1, "hints must accumulate");
        // Add an unrelated note and a snapshot, then undo it without losing hints.
        bool placed = false;
        for (int row = 0; row < BOARD_SIZE && !placed; ++row)
            for (int col = 0; col < BOARD_SIZE && !placed; ++col)
                if (state.numbers[row][col] == 0) {
                    notes.setSpecificColorCode(row, col, 1, 'g');
                    placed = true;
                }
        remember(memory, capture(board, notes));
        undo(memory, board, notes);
        state = capture(board, notes);
        require(hintCount(state) == i + 1, "note undo preserves every consumed hint");
        require(!changed(memory, state), "undo result is stable in the frame loop");
    }
    board.restartCurrentNumberGrid();
    notes.resetWholeNotepadColorCode();
    require(hintCount(capture(board, notes)) == 3, "reset keeps all three hints");
    memory.removeAllMemory();
    board.createSolidPieces(IMPOSSIBLE);
    State fresh = capture(board, notes);
    int clues = 0;
    for (auto& row : fresh.numbers) for (int value : row) clues += value != 0;
    require(clues == 17 && hintCount(fresh) == 0, "Impossible uses packaged dataset and has fresh hints");
    require(board.isCurrentBoardUnique(), "packaged Impossible board remains uniquely solvable");
}
}

int main() {
    try {
        testPauseTimer();
        testWindowLayout();
        // Repeating across random puzzles catches dependence on hint location.
        for (int i = 0; i < 10; ++i) {
            testHintUndoAndReset();
            testMultipleHintsAndNotes();
        }
        std::cout << "PASS: timer, window sizing, fixed hints, repeated undo, notes, reset and new-game regressions\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
