find_package(GDBusCodegen REQUIRED)
include(TargetModules)
include(InstallConfiguredFile)
include(DBusInstallDirs)

install_configured_file(Service.service.in ${DBUS_BUS_NAME}.service
                        DESTINATION ${DBUS_INSTALL_SYSTEM_SERVICES_DIR})

install_configured_file(Policy.conf.in ${DBUS_BUS_NAME}.conf
                        DESTINATION ${DBUS_INSTALL_SYSTEM_POLICIES_DIR})

install_configured_file(Controls.xml.in ${DBUS_BUS_NAME}.Controls.xml
                        DESTINATION ${DBUS_INSTALL_INTERFACES_DIR})

gdbus_generate_code(${CMAKE_CURRENT_BINARY_DIR}/${DBUS_BUS_NAME}.Controls.xml
                    PREFIX ${DBUS_BUS_NAME}
                    NAMESPACE DBus
                    PRAGMA_ONCE)

add_library(controld-dbus STATIC DBusControls.c)
target_required_modules(controld-dbus PUBLIC gio-unix-2.0)
target_include_directories(controld-dbus INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(controld-dbus PRIVATE -Wno-unused-parameter)
