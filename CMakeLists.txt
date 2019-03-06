cmake_minimum_required(VERSION 3.13)

include(${CMAKE_SOURCE_DIR}/cmake/Config.cmake)

project(${PROJECT_NAME}
        DESCRIPTION "A keyboard function keys daemon."
        HOMEPAGE_URL "https://github.com/neimad/kdbfnd"
        VERSION 0.1
        LANGUAGES C)

include(TargetModules)

require_module(gio-2.0 MINIMUM 2.58)
require_module(gio-unix-2.0 MINIMUM 2.58)
require_module(libudev MINIMUM 240)
require_module(libinput MINIMUM 1.12)

add_subdirectory(dbus)
add_subdirectory(src)
add_subdirectory(systemd)
