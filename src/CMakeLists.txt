include(TargetModules)
include(GNUInstallDirs)

set(target ${PROJECT_NAME})
set(units KeyboardsSource DBusService)

configure_file(config.h.in config.h)

add_library(${target}-units STATIC ${units})
target_required_modules(${target}-units MODULES gio-2.0 libudev libinput)
target_include_directories(${target}-units BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                                                         ${CMAKE_CURRENT_BINARY_DIR}/../dbus)
target_link_libraries(${target}-units ${target}-dbus)

add_executable(${target} main.c)
target_required_modules(${target} MODULES gio-2.0)
target_link_libraries(${target} ${target}-units)

install(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_BINDIR})
