cmake_minimum_required(VERSION 3.10.0)
project(Qanvas VERSION 0.1.0 LANGUAGES C CXX)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# *********************************************************************************************
#                                         CMAKE OUTPUT
# *********************************************************************************************

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/bin/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/lib)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/cmake)

# *********************************************************************************************
#                                       REQUIRED PACKAGES
# *********************************************************************************************

# TEMP
find_path(STB_INCLUDE_DIR NAMES stb/stb_image.h)
if(NOT STB_INCLUDE_DIR)
    message(FATAL_ERROR "stb_image.h not found! Install it via your package manager.")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIODCXX REQUIRED libgpiod)

# *********************************************************************************************
#                                      LOCATION DEFINITIONS
# *********************************************************************************************

set(INCLUDE_DIR ${PROJECT_DIR}/include)
set(SRC_DIR ${PROJECT_DIR}/src)
set(EXTERNAL_DIR ${PROJECT_DIR}/external)

set(CORE_DIR ${SRC_DIR}/qanvas/core)
set(DRAW_DIR ${SRC_DIR}/qanvas/draw)
set(COMMAND_DIR ${SRC_DIR}/qanvas/command)
set(DEVICE_DIR ${SRC_DIR}/qanvas/device)
set(RESOURCES_DIR ${SRC_DIR}/qanvas/resources)
set(PLATFORM_DIR ${SRC_DIR}/qanvas/platform)
set(WIDGETS_DIR ${SRC_DIR}/qanvas/widgets)

# *********************************************************************************************
#                                         Subdirectories
# *********************************************************************************************

add_subdirectory(${EXTERNAL_DIR}/Qontext)

# *********************************************************************************************
#                                       SOURCE DEFINITIONS
# *********************************************************************************************

set(CORE_SRC
    ${CORE_DIR}/identity_transform.cpp
    ${CORE_DIR}/scroll_transform.cpp
    ${CORE_DIR}/color.cpp
    ${CORE_DIR}/background.cpp
    ${CORE_DIR}/padding.cpp
)

set(DRAW_SRC
    ${DRAW_DIR}/canvas.cpp
    ${DRAW_DIR}/canvas_view.cpp
    ${DRAW_DIR}/compositor.cpp
    ${DRAW_DIR}/renderer.cpp
)

set(COMMAND_SRC
    ${COMMAND_DIR}/blit.cpp
    ${COMMAND_DIR}/scroll_sync.cpp
    ${COMMAND_DIR}/mirror_barrier.cpp
    ${COMMAND_DIR}/command_buffer.cpp
)

set(DEVICE_SRC
    ${DEVICE_DIR}/device.cpp
    ${DEVICE_DIR}/scroll_register.cpp
)

set(RESOURCES_SRC
    ${RESOURCES_DIR}/image.cpp
    ${RESOURCES_DIR}/font.cpp
    ${RESOURCES_DIR}/text.cpp
)

set(PLATFORM_SRC
    ${PLATFORM_DIR}/gpio/pin.cpp
    ${PLATFORM_DIR}/gpio/pin_manager.cpp
    ${PLATFORM_DIR}/spi/spi_bus.cpp
    ${PLATFORM_DIR}/spi/spi_transport.cpp
    ${PLATFORM_DIR}/spi/drivers/st7796_driver.cpp
)

set(WIDGETS_SRC
    ${WIDGETS_DIR}/navigator.cpp
    ${WIDGETS_DIR}/widget.cpp
    ${WIDGETS_DIR}/panel.cpp
    ${WIDGETS_DIR}/container.cpp
    ${WIDGETS_DIR}/box_container.cpp
    ${WIDGETS_DIR}/scroll_container.cpp
    ${WIDGETS_DIR}/fill.cpp
    ${WIDGETS_DIR}/button.cpp
    ${WIDGETS_DIR}/label.cpp
    ${WIDGETS_DIR}/slider.cpp
    ${WIDGETS_DIR}/text_field.cpp
    ${WIDGETS_DIR}/overlay.cpp
    ${WIDGETS_DIR}/keyboard.cpp
    ${WIDGETS_DIR}/grid_keyboard.cpp
)

set(SOURCES
    ${CORE_SRC}
    ${DRAW_SRC}
    ${DEVICE_SRC}
    ${COMMAND_SRC}
    ${RESOURCES_SRC}
    ${PLATFORM_SRC}
    ${WIDGETS_SRC}
)

add_library(Qanvas STATIC ${SOURCES})

add_executable(QanvasDebug ${SOURCES} debug.cpp)

# *********************************************************************************************
#                                            INCLUDE
# *********************************************************************************************

set(PROJECT_INCLUDE_DIRS 
    ${INCLUDE_DIR}
)

set(PACKAGES_INCLUDE_DIRS
    ${GPIOD_INCLUDE_DIRS}
)

target_include_directories(Qanvas PRIVATE
    ${PROJECT_INCLUDE_DIRS}
    ${PACKAGES_INCLUDE_DIRS}
    ${EXTERNAL_DIR}/Qontext/include
)

target_include_directories(QanvasDebug PRIVATE
    ${PROJECT_INCLUDE_DIRS}
    ${PACKAGES_INCLUDE_DIRS}
    ${EXTERNAL_DIR}/Qontext/include
)

# *********************************************************************************************
#                                           LINKING
# *********************************************************************************************

set(PROJECT_LIBRARIES
    ${GPIODCXX_LIBRARIES}
)

target_link_libraries(Qanvas PRIVATE
    ${PROJECT_LIBRARIES}
    Qontext
)
target_link_libraries(QanvasDebug Qanvas)

# *********************************************************************************************
#                                       COMPILE OPTIONS
# *********************************************************************************************

set(PROJECT_COMPILE_OPTIONS)

target_compile_options(Qanvas PRIVATE ${PROJECT_COMPILE_OPTIONS})

# *********************************************************************************************
#                                     COMPILE DEFINITIONS
# *********************************************************************************************

target_compile_definitions(Qanvas PRIVATE)