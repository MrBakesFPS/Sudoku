# MySudoku

A Sudoku game by Tyson Koopman-Baker, built with C++17 and SFML 2.

## Requirements

- A C++17 compiler (for example, Visual Studio 2022, GCC, or Clang)
- CMake 3.21 or newer
- **SFML 2.5 or newer in the 2.x series**, including graphics, window, and system
  development libraries. **SFML 3 is not supported.** SFML 2.6.2 is a suitable choice.
- A graphical desktop and OpenGL support to play; the regression tests do not open a window

The CMake build is the portable, recommended build. It does not download dependencies.

## Linux

On Debian/Ubuntu releases providing SFML 2.x:

```sh
sudo apt update
sudo apt install build-essential cmake libsfml-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target run
```

If your distribution supplies SFML 3, install/build SFML 2.x separately. For a
non-system installation, add `-DCMAKE_PREFIX_PATH=/path/to/SFML-2.6.2` when configuring.
Alternatively, set `-DSFML_DIR=/path/to/directory/containing/SFMLConfig.cmake`.
Keep the matching shared libraries on your system's library search path at runtime.

## Windows (Visual Studio 2022)

1. Install Visual Studio's **Desktop development with C++** workload and CMake tools.
2. Download the [SFML 2.6.2 SDK](https://www.sfml-dev.org/download/sfml/2.6.2/)
   matching your compiler and target architecture. For the commands below, use
   **Visual C++ 17 (2022), 64-bit**, and extract it to `C:\Libraries\SFML-2.6.2`.
3. From a Developer PowerShell in this repository, run:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Libraries/SFML-2.6.2"
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
cmake --build build --config Release --target run
```

Use `Debug` consistently instead of `Release` for a debug build. CMake selects and
copies the matching SFML DLLs beside `Sudoku.exe`. Do not replace them with the old
DLLs checked into the repository root: their architecture/configuration may differ.
See [SFML's Visual Studio guide](https://www.sfml-dev.org/tutorials/2.6/start-vc.php)
for compiler and library compatibility details.

You can open the generated `build/MySudoku.sln` in Visual Studio. Its Sudoku target
uses the executable directory as its debugger working directory. The original
`Sudoku.sln` remains a legacy project with machine-specific SFML/toolset settings;
it is not needed for the CMake build.

## Assets and launching

The game reads these files from its **current working directory**:

- `JumpersCondensedItalic-d9zq7.ttf`: required font
- `17_clue_boards.txt`: required for Impossible difficulty
- `icon.png`: optional window icon

CMake copies the assets beside the executable on every build. The `run` target
sets the correct working directory automatically. To launch manually:

```sh
# Linux (single-configuration build)
cd build/bin
./Sudoku
```

```powershell
# Windows (Visual Studio Release build)
Set-Location build/bin/Release
.\Sudoku.exe
```

For an IDE or desktop shortcut, set its working directory / "Start in" to that
same executable directory. A missing font or Impossible-puzzle file usually means
the working directory is wrong. The Windows executable resource icon is optional:
place an `icon.ico` in `Sudoku/` and reconfigure to embed it. Likewise, reconfigure
after adding an optional `Sudoku/icon.png` so it is copied and packaged.

## Controls and behavior

- Menu: `Tab` changes difficulty; `Space` starts a puzzle
- Move the selection: arrow keys or `W`, `A`, `S`, `D`
- Enter a number/note: `1`–`9` (top row or numeric keypad)
- Clear the selected entry/notes: `Backspace` or `0`
- `Space`: switch between number entry and notes
- `H`: reveal a hint (up to three per puzzle)
- `U`: undo; `R`: reset player entries; `M`: return to the menu
- `Esc`: pause/resume and show controls; paused time does not count toward play time

The window fits smaller desktops and preserves the board's proportions when
resized. Revealed hints become locked cells. Undo/reset retain those hints and
their consumed hint budget; starting a new puzzle restores the budget.

## Tests

CTest runs `sudoku_regressions`, covering the board/history and extracted timer/layout
logic without opening a graphical window. Tests are enabled by default; configure
with `-DBUILD_TESTING=OFF` if you only want the game.

## Install or package

From the repository root, after building:

```sh
cmake --install build --config Release --prefix "$PWD/stage"
cpack --config build/CPackConfig.cmake -C Release -B dist
```

For PowerShell, use `--prefix "$((Get-Location).Path)/stage"`. The install directory
and ZIP (Windows) or `.tar.gz` (Linux) package contain the executable, assets, and
this README together. Launch from that extracted/installed directory as described
above. Configure/build each target platform separately; packages are not cross-platform.

Windows packages include the selected SFML graphics/window/system DLLs. The matching
Microsoft Visual C++ runtime (or your MinGW compiler's runtime DLLs) is still required.
Linux packages rely on compatible installed SFML 2.x and system libraries; they do
not bundle those dependencies. Include applicable SFML/dependency/font license
notices if redistributing a package. These archives are local application bundles,
not system-wide installers.
