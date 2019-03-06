#pragma once

#include <glib.h>
#include <libinput.h>
#include <linux/input-event-codes.h>

/**
 * KeyboardSource:
 *
 * A keyboard events source.
 */
typedef struct _KeyboardsSource KeyboardsSource;

/**
 * KeyCode:
 *
 * A key code.
 */
typedef uint32_t KeyCode;

/**
 * KeyState:
 * @KEY_STATE_PRESSED: the key is pressed
 * @KEY_STATE_RELEASED: the key is released
 *
 * A key state.
 */
typedef enum _KeyState {
  KEY_STATE_PRESSED  = LIBINPUT_KEY_STATE_PRESSED,
  KEY_STATE_RELEASED = LIBINPUT_KEY_STATE_RELEASED,
} KeyState;

/**
 * KeyboardsSourceFunc:
 * @code: the key code
 * @state: the key state
 * @user_data: the data passed to the handler
 *
 * The callback function type for #KeyboardsSource.
 */
typedef void (*KeyboardsSourceFunc)(KeyCode code,
                                    KeyState state,
                                    gpointer user_data);

/**
 * keyboards_source_new:
 *
 * Creates a keyboards event source.
 *
 * Returns: (transfer full): a new #KeyboardsSource
 */
GSource *keyboards_source_new(void);
