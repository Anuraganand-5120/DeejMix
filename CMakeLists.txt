cmake_minimum_required(VERSION 3.16)

project(DeejMix VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql SerialPort Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql SerialPort Network REQUIRED)

add_executable(DeejMix
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/mainwindow.ui
  src/databasemanager.h
  src/databasemanager.cpp
  src/hardwaremanager.h
  src/hardwaremanager.cpp
  src/audiomanager.h
  src/audiomanager.cpp
  src/channelstrip.h
  src/channelstrip.cpp
  src/knobwidget.h
  src/knobwidget.cpp
  resources/app.qrc
)

target_link_libraries(DeejMix PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Network)
if(WIN32)
    target_link_libraries(DeejMix PRIVATE ole32 uuid psapi shell32)
endif()

# Set icon (to be added later)
# if(WIN32)
#   target_sources(DeejMix PRIVATE resources/app.rc)
# endif()
