list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/shared-modules")

set(PROJECT_NAME kbdfnd)
set(DBUS_BUS_NAME "org.neimad.${PROJECT_NAME}1")
set(DBUS_OBJECT_PATH "/org/neimad/${PROJECT_NAME}1")
