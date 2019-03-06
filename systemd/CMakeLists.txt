include(InstallConfiguredFile)
include(SystemdInstallDirs)

set(SERVICE_EXECUTABLE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}")

install_configured_file(Service.service.in ${PROJECT_NAME}.service
                        DESTINATION ${SYSTEMD_INSTALL_SYSTEM_UNITS_DIR})
