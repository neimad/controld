# controld

**controld** is a daemon which notify through D-Bus about control events triggered by the special keys.

## D-Bus interface

The events are notified through D-Bus signals declared in a D-Bus interface (see [the `dist` directory](dist)).

## Supported controls

The following controls are supported:

 - screen backlight,
 - audio volume,
 - media player,
 - search,
 - airplane mode (RF kill).
