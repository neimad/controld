find_package(GDBusCodegen REQUIRED)
include(TargetModules)
include(InstallConfiguredFile)
include(DBusInstallDirs)

set(target ${PROJECT_NAME}-dbus)

install_configured_file(Service.service.in ${DBUS_BUS_NAME}.service
                        DESTINATION ${DBUS_INSTALL_SYSTEM_SERVICES_DIR})

install_configured_file(Policy.conf.in ${DBUS_BUS_NAME}.conf
                        DESTINATION ${DBUS_INSTALL_SYSTEM_POLICIES_DIR})

install_configured_file(Functions.xml.in ${DBUS_BUS_NAME}.Functions.xml
                        DESTINATION ${DBUS_INSTALL_INTERFACES_DIR})

gdbus_generate_code(${CMAKE_CURRENT_BINARY_DIR}/${DBUS_BUS_NAME}.Functions.xml
                    PREFIX ${DBUS_BUS_NAME}
                    NAMESPACE Kbdbus
                    PRAGMA_ONCE)

add_library(${target} STATIC KbdbusFunctions.c)
target_required_modules(${target} MODULES gio-unix-2.0)
target_compile_options(${target} PUBLIC -Wno-unused-parameter)
