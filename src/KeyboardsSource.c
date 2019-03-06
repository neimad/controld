#include "KeyboardsSource.h"

#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <glib-unix.h>
#include <unistd.h>

#define SOURCE_CHECK_FUNC(f) (gboolean(*)(GSource *)) f
#define SOURCE_DISPATCH_FUNC(f) \
  (gboolean(*)(GSource *, GSourceFunc, gpointer)) f
#define SOURCE_FINALIZE_FUNC(f) (void (*)(GSource *)) f

struct _KeyboardsSource
{
  GSource parent;
  struct udev *udev;
  struct libinput *libinput;
};

/*
 * open_device:
 * Opens the device located at the specified path.
 *
 * See #libinput_interface.open_restricted.
 */
static int
open_device(const gchar *path, gint flags, void *user_data G_GNUC_UNUSED)
{
  int fd;

  g_return_val_if_fail(path != NULL, -EINVAL);

  fd = open(path, flags);

  if (fd == -1)
  {
    g_error("Failed to open device %s: %s.", path, g_strerror(errno));

    return -errno;
  }

  g_debug("Opened device %s at file descriptor %d.", path, fd);

  return fd;
}

/*
 * close_device:
 * Closes the device associated to the given file descriptor.
 *
 * See #libinput_interface.close_restricted.
 */
static void close_device(gint fd, void *user_data G_GNUC_UNUSED)
{
  if (close(fd) == -1)
  {
    g_error("Failed to close device file descriptor %d: %s.",
            fd,
            g_strerror(errno));
  }

  g_debug("Closed device file descriptor %d.", fd);
}

static struct libinput_interface libinput_interface = {
  .open_restricted  = open_device,
  .close_restricted = close_device,
};

/*
 * keyboards_source_check:
 * @self: the source
 *
 * Checks if some keyboard events are available.
 *
 * Events are dispatched within _libinput_, then non-keyboard events are
 * discarded.
 *
 * Returns: TRUE on the first keyboard event found.
 */
static gboolean keyboards_source_check(KeyboardsSource *self)
{
  if (libinput_dispatch(self->libinput) < 0)
  {
    g_error("Failed to dispatch the input devices events: %s.",
            g_strerror(errno));
  }

  while (libinput_next_event_type(self->libinput) != LIBINPUT_EVENT_NONE &&
         libinput_next_event_type(self->libinput) !=
           LIBINPUT_EVENT_KEYBOARD_KEY)
  {
    libinput_event_destroy(libinput_get_event(self->libinput));
  }

  return libinput_next_event_type(self->libinput) ==
         LIBINPUT_EVENT_KEYBOARD_KEY;
}

/*
 * keyboards_source_dispatch:
 * @self: the source
 * @handler: the handler to call
 * @user_data: the data passed to the handler
 *
 * Dispatches the keyboard events.
 *
 * The given handler is invoked with the key code, the key state, and the given
 * user data as parameters.
 *
 * Returns: %G_SOURCE_CONTINUE
 */
static gboolean keyboards_source_dispatch(KeyboardsSource *self,
                                          KeyboardsSourceFunc handler,
                                          gpointer user_data)
{
  struct libinput_event *event                   = NULL;
  struct libinput_event_keyboard *keyboard_event = NULL;
  KeyCode code;
  enum libinput_key_state state;

  g_return_val_if_fail(handler != NULL, G_SOURCE_REMOVE);

  while (libinput_next_event_type(self->libinput) != LIBINPUT_EVENT_NONE)
  {
    event = libinput_get_event(self->libinput);
    g_assert(event != NULL);

    if (libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY)
    {
      keyboard_event = libinput_event_get_keyboard_event(event);
      g_assert(keyboard_event != NULL);

      state = libinput_event_keyboard_get_key_state(keyboard_event);
      code  = libinput_event_keyboard_get_key(keyboard_event);

      handler(code, state, user_data);
    }

    libinput_event_destroy(event);
  }

  return G_SOURCE_CONTINUE;
}

static void keyboards_source_finalize(KeyboardsSource *self)
{
  libinput_unref(self->libinput);
  udev_unref(self->udev);
}

static GSourceFuncs keyboards_source_funcs = {
  .prepare  = NULL,
  .check    = SOURCE_CHECK_FUNC(keyboards_source_check),
  .dispatch = SOURCE_DISPATCH_FUNC(keyboards_source_dispatch),
  .finalize = SOURCE_FINALIZE_FUNC(keyboards_source_finalize),
};

static void keyboards_source_init(KeyboardsSource *self)
{
  g_return_if_fail(self != NULL);

  self->udev = udev_new();

  if (self->udev == NULL)
  {
    g_error("Failed to create a udev context.");
  }

  self->libinput =
    libinput_udev_create_context(&libinput_interface, NULL, self->udev);

  if (self->libinput == NULL)
  {
    g_error("Failed to create a libinput context.");
  }

  if (libinput_udev_assign_seat(self->libinput, "seat0") == -1)
  {
    g_error("Failed to assign a seat to the libinput context.");
  }

  g_source_add_unix_fd((GSource *) self,
                       libinput_get_fd(self->libinput),
                       G_IO_IN | G_IO_HUP | G_IO_ERR);
}

GSource *keyboards_source_new(void)
{
  GSource *source                   = NULL;
  KeyboardsSource *keyboards_source = NULL;

  source = g_source_new(&keyboards_source_funcs, sizeof(*keyboards_source));
  keyboards_source = (KeyboardsSource *) source;

  g_source_set_name(source, "keyboards source");

  keyboards_source_init(keyboards_source);

  return source;
}
