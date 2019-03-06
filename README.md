# kbdfnd
_Keyboard Function keys Daemon_


**kbdfnd** is a daemon which notify through D-Bus about control events triggered by the function keys (using the ``Fn`` key).

## Supported functions

The following functions are supported:

 - screen backlight,
 - audio volume,
 - media player,
 - search,
 - airplane mode (RF kill).

## D-Bus interface

The events are notified through D-Bus signals declared in a [D-Bus interface][dbus-interface].

[dbus-interface]: dbus/Functions.xml.in
