# CMakeList.txt : CMake project for snake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)
set(CMAKE_C_STANDARD 99)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (snake C)

# Add source to this project's executable.
add_executable (snake  "src/snake.h" "src/main.c" "external/raylib/include/raylib.h" "src/snake.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET snake PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(snake PRIVATE
    ${CMAKE_SOURCE_DIR}/external/raylib/include
)

target_link_directories(snake PRIVATE
    ${CMAKE_SOURCE_DIR}/external/raylib/lib
)

target_link_libraries(snake
    raylib
    opengl32
    gdi32
    winmm
)

# TODO: Add tests and install targets if needed.
