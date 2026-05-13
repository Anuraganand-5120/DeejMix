# DeejMix

DeejMix is a premium, high-performance Windows desktop application that bridges physical hardware knobs/sliders (like an Arduino-based Deej setup) with your system's audio mixer. 

Built with C++ and Qt 6, it features a highly polished, professional glassmorphic UI reminiscent of high-end studio audio interfaces.

## Features
- **Real-Time Hardware Sync**: Instantly detects and synchronizes with Arduino hardware over serial (COM ports).
- **Per-Application Volume Control**: Dynamically fetch and assign running Windows applications to specific physical dials.
- **Premium User Interface**: 
  - Dual modes: **Slider Mode** (Blue Neon Theme) and **Hardware Knob Mode** (Purple Neon Theme).
  - Cinematic lighting, drop shadows, and glassmorphism.
  - Realistic 3D-rendered, mechanical knob widgets with dynamic LED tracking.
- **Automatic Process Tracking**: Automatically extracts high-resolution `.exe` icons directly from the Windows system using Win32 APIs.
- **Persistent Settings**: Saves all assignments and calibration settings locally using an embedded SQLite database.
- **Background Mode**: System tray integration for seamless background operation.

## Technologies Used
- C++17
- Qt 6 (Core, Gui, Widgets, Sql, SerialPort, Network)
- Windows Core Audio APIs (ISimpleAudioVolume, IAudioSessionManager2)
- Win32 API (Process enumeration, Icon extraction)
- CMake

## Building from Source

### Prerequisites
- CMake 3.16+
- Qt 6 (MinGW 64-bit toolchain recommended)
- Windows 10/11

### Compilation
Open the project in Qt Creator or build via CMake:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Deployment
A `deploy.ps1` script is included to automatically compile a release build and use `windeployqt` to bundle all necessary DLLs for standalone sharing.

## Hardware Requirements
This software is designed to communicate with microcontrollers (like Arduino Nano/Uno) running standard Deej firmware, sending serial data in the format: `val|val|val|val\r\n`.

## Credits

DeejMix is inspired by the original deej project, an open-source hardware volume mixer that allows physical sliders to control individual app volumes on Windows. This project builds upon that idea with a redesigned modern UI, advanced hardware visualization, profiles, calibration tools, and a more polished desktop experience.

Original Deej Project: https://github.com/omriharel/deej

