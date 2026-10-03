cmake_minimum_required(VERSION 3.21)
project(MySudoku VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

# The game uses the SFML 2 API. Its package config also checks the major version.
find_package(SFML 2.5 CONFIG REQUIRED COMPONENTS graphics window system)
if(SFML_VERSION VERSION_GREATER_EQUAL 3 OR NOT TARGET sfml-graphics)
    message(FATAL_ERROR "MySudoku requires SFML 2.5 or newer within the 2.x series (not SFML 3).")
endif()

add_library(sudoku_core STATIC
    Sudoku/Board.cpp
    Sudoku/GameMemory.cpp
    Sudoku/Highlighter.cpp
    Sudoku/Notepad.cpp
)
target_compile_features(sudoku_core PUBLIC cxx_std_17)
target_include_directories(sudoku_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku")

add_executable(Sudoku Sudoku/Sudoku.cpp Sudoku/Game.cpp)
target_link_libraries(Sudoku PRIVATE sudoku_core sfml-graphics sfml-window sfml-system)
set_target_properties(Sudoku PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Sudoku>"
)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sudoku)

# A native Windows icon is optional; the original checkout does not provide one.
if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku/icon.ico")
    enable_language(RC)
    target_sources(Sudoku PRIVATE Sudoku/resource.rc)
endif()

set(SUDOKU_ASSETS
    "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku/JumpersCondensedItalic-d9zq7.ttf"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku/17_clue_boards.txt"
)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku/icon.png")
    list(APPEND SUDOKU_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku/icon.png")
endif()

# Run even when no C++ file changed, so changed or deleted asset copies recover.
add_custom_target(sudoku_assets
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:Sudoku>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${SUDOKU_ASSETS} "$<TARGET_FILE_DIR:Sudoku>"
    DEPENDS ${SUDOKU_ASSETS}
    COMMENT "Copying Sudoku runtime assets"
    VERBATIM
)
add_dependencies(Sudoku sudoku_assets)

# Copy the DLLs from the selected SFML package, never the legacy root DLLs.
# Imported targets choose the matching architecture and Debug/Release files.
if(WIN32)
    foreach(sfml_target IN ITEMS sfml-graphics sfml-window sfml-system)
        get_target_property(sfml_type ${sfml_target} TYPE)
        if(sfml_type STREQUAL "SHARED_LIBRARY")
            add_custom_command(TARGET Sudoku POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:${sfml_target}>" "$<TARGET_FILE_DIR:Sudoku>"
                VERBATIM
            )
            install(IMPORTED_RUNTIME_ARTIFACTS ${sfml_target} RUNTIME DESTINATION .)
        endif()
    endforeach()
endif()

add_custom_target(run
    COMMAND "${CMAKE_COMMAND}" -E chdir "$<TARGET_FILE_DIR:Sudoku>" "$<TARGET_FILE:Sudoku>"
    DEPENDS Sudoku
    USES_TERMINAL
    VERBATIM
)

include(CTest)
if(BUILD_TESTING)
    add_executable(sudoku_regression_tests tests/RegressionTests.cpp)
    target_link_libraries(sudoku_regression_tests PRIVATE sudoku_core)
    add_test(NAME sudoku_regressions COMMAND sudoku_regression_tests)
    set_tests_properties(sudoku_regressions PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Sudoku"
    )
endif()

# Keep a relocatable app directory: the game resolves assets from its cwd.
install(TARGETS Sudoku RUNTIME DESTINATION .)
install(FILES ${SUDOKU_ASSETS} README.md DESTINATION .)
set(CPACK_PACKAGE_NAME MySudoku)
set(CPACK_PACKAGE_VENDOR "Tyson Koopman-Baker")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A C++17 and SFML 2 Sudoku game")
if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()
include(CPack)
