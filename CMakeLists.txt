cmake_minimum_required(VERSION 3.16)

project(LinuxProcessHacker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    ProcessInfo.cpp
    ProcessInfo.h
    ProcessReader.cpp
    ProcessReader.h
    SystemStatReader.cpp
    SystemStatReader.h
    MemInfoReader.cpp
    MemInfoReader.h
    ProcessTableModel.cpp
    ProcessTableModel.h
    ProcFsUtils.cpp
    ProcFsUtils.h
    NetworkConnectionInfo.cpp
    NetworkConnectionInfo.h
    NetworkAddressParser.cpp
    NetworkAddressParser.h
    SocketInodeResolver.cpp
    SocketInodeResolver.h
    NetworkConnectionReader.cpp
    NetworkConnectionReader.h
    NetworkTableModel.cpp
    NetworkTableModel.h
    ProcessSignalSender.cpp
    ProcessSignalSender.h
    RootkitDetector.cpp
    RootkitDetector.h
    RootkitScanWorker.cpp
    RootkitScanWorker.h
    MemoryRegion.cpp
    MemoryRegion.h
    MemoryMapsParser.cpp
    MemoryMapsParser.h
    ProcessMemoryDumper.cpp
    ProcessMemoryDumper.h
    MemoryDumpDialog.cpp
    MemoryDumpDialog.h
    MemoryDumpDialog.ui
    AutostartEntry.cpp
    AutostartEntry.h
    DesktopFileParser.cpp
    DesktopFileParser.h
    AutostartManager.cpp
    AutostartManager.h
    AutostartTableModel.cpp
    AutostartTableModel.h
)

qt_add_executable(LinuxProcessHacker
    ${PROJECT_SOURCES}
)

target_link_libraries(LinuxProcessHacker
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
)

target_include_directories(LinuxProcessHacker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(LinuxProcessHacker PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE
)

install(TARGETS LinuxProcessHacker
    RUNTIME DESTINATION bin
)
