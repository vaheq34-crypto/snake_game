cmake_minimum_required(VERSION 4.3)
project(snake_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(snake_game main.cpp style.css)
target_link_libraries(snake_game
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

