include(${CMAKE_CURRENT_LIST_DIR}/../cmake/Config.cmake)

configure_file(${CMAKE_CURRENT_LIST_DIR}/../dbus/Controls.xml.in
               ${CMAKE_CURRENT_LIST_DIR}/${DBUS_BUS_NAME}.Controls.xml)
