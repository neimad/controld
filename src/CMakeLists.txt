include(TargetModules)
include(GNUInstallDirs)

configure_file(config.h.in config.h)

add_library(controld-units STATIC KeyboardsSource.c
                                  DBusService.c)
target_required_modules(controld-units PUBLIC gio-2.0 libudev libinput)
target_link_libraries(controld-units controld-dbus)

add_executable(controld main.c)
target_include_directories(controld PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(controld controld-units)

install(TARGETS controld DESTINATION ${CMAKE_INSTALL_BINDIR})
