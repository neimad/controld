// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of controld.

#include "DBusService.h"
#include "KeyboardsSource.h"
#include "config.h"

#include <glib-unix.h>

typedef struct _Application
{
  GMainLoop *loop;
  DBusService *service;
  guint keyboards_source_id;
} Application;

void
handle_key_change(KeyCode code, KeyState state, DBusService *service)
{
  gchar *signal_name = NULL;

#define KEY(KeyCode, OnKeyPressed, OnKeyReleased) \
  case KeyCode:                                   \
    switch (state)                                \
    {                                             \
      case KEY_STATE_PRESSED:                     \
        OnKeyPressed;                             \
        break;                                    \
      case KEY_STATE_RELEASED:                    \
        OnKeyReleased;                            \
        break;                                    \
    }                                             \
    break;

#define SIGNAL(Name) signal_name = Name;
#define NOT_HANDLED

  switch (code)
  {
    KEY(KEY_BRIGHTNESSDOWN, SIGNAL("screen-backlight-decrease"), NOT_HANDLED);
    KEY(KEY_BRIGHTNESSUP, SIGNAL("screen-backlight-increase"), NOT_HANDLED);
    KEY(KEY_VOLUMEDOWN, SIGNAL("volume-decrease"), NOT_HANDLED);
    KEY(KEY_VOLUMEUP, SIGNAL("volume-increase"), NOT_HANDLED);
    KEY(KEY_MUTE, SIGNAL("volume-mute"), NOT_HANDLED);
    KEY(KEY_PLAYPAUSE, SIGNAL("media-player-play-pause"), NOT_HANDLED);
    KEY(KEY_PREVIOUSSONG, SIGNAL("media-player-previous"), NOT_HANDLED);
    KEY(KEY_NEXTSONG, SIGNAL("media-player-next"), NOT_HANDLED);
    KEY(KEY_SEARCH, SIGNAL("search"), NOT_HANDLED);
    KEY(KEY_RFKILL, SIGNAL("airplace-mode"), NOT_HANDLED);
  }

#undef KEY
#undef SIGNAL
#undef NOT_HANDLED

  if (signal_name != NULL)
  {
    dbus_service_emit_signal(service, signal_name);
  }
}

void
init_application(GDBusConnection *connection,
                 const gchar *name G_GNUC_UNUSED,
                 Application *app)
{
  GSource *source = NULL;

  g_message("Connected to D-Bus as %s.",
            g_dbus_connection_get_unique_name(connection));

  app->service = dbus_service_new(DBUS_OBJECT_PATH);
  dbus_service_register(app->service, connection);
  g_info("Registered D-Bus service on %s.", DBUS_OBJECT_PATH);

  source = keyboards_source_new();
  g_source_set_callback(source,
                        G_SOURCE_FUNC(handle_key_change),
                        app->service,
                        NULL);
  app->keyboards_source_id = g_source_attach(source, NULL);
  g_clear_pointer(&source, g_source_unref);
  g_message("Listening on keyboards...");
}

void
run_application(GDBusConnection *connection G_GNUC_UNUSED,
                const gchar *name,
                Application *app G_GNUC_UNUSED)
{
  g_message("Acquired D-Bus name %s.", name);
}

void
quit_application(Application *app)
{
  g_message("Exiting...");

  g_info("Releasing resources...");

  g_clear_object(&app->service);
  g_info("Destroyed D-Bus service.");

  g_source_remove(app->keyboards_source_id);
  g_info("Removed keyboards source.");

  g_main_loop_unref(app->loop);
  g_main_loop_quit(app->loop);
}

void
cancel_application(GDBusConnection *connection,
                   const gchar *name,
                   Application *app)
{
  if (connection == NULL)
  {
    g_error("Failed to connect to D-Bus.");
  }
  else
  {
    g_error("Failed to acquire D-Bus name %s.", name);
  }

  quit_application(app);
}

int
main(void)
{
  Application app = {NULL};

  g_unix_signal_add(SIGHUP, G_SOURCE_FUNC(quit_application), &app);
  g_unix_signal_add(SIGINT, G_SOURCE_FUNC(quit_application), &app);
  g_unix_signal_add(SIGTERM, G_SOURCE_FUNC(quit_application), &app);
  g_info("Installed Unix signals handler.");

  app.loop = g_main_loop_new(NULL, FALSE);

  g_message("Acquiring D-Bus name %s on system bus...", DBUS_BUS_NAME);
  g_bus_own_name(G_BUS_TYPE_SYSTEM,
                 DBUS_BUS_NAME,
                 G_BUS_NAME_OWNER_FLAGS_DO_NOT_QUEUE,
                 (GBusAcquiredCallback) init_application,
                 (GBusNameAcquiredCallback) run_application,
                 (GBusNameLostCallback) cancel_application,
                 &app,
                 NULL);

  g_main_loop_run(app.loop);
}
