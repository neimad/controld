// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of controld.

#pragma once

#include <gio/gio.h>

/**
 * DBusService:
 * The D-Bus service of the application.
 */
G_DECLARE_FINAL_TYPE(DBusService, dbus_service, DBUS, SERVICE, GObject)
#define DBUS_TYPE_SERVICE dbus_service_get_type()

/**
 * dbus_service_new: (constructor)
 * @object_path: (type filename): the object path
 *
 * Creates a D-Bus service located at the specified @object_path.
 *
 * Returns: (transfer full): a new #DBusService
 */
DBusService *dbus_service_new(const gchar *object_path);

/**
 * dbus_service_set_connection: (method)
 * @service: the service
 * @connection: the D-Bus connection
 *
 * Registers the service on a D-Bus connection.
 */
void dbus_service_register(DBusService *service, GDBusConnection *connection);

/**
 * dbus_service_emit_signal: (method)
 * @service: the service
 * @signal_name: the name of the signal to emit
 *
 * Emits a signal on the bus through the service.
 *
 * The signal name must be formated as for GObject signals.
 */
void dbus_service_emit_signal(DBusService *service, const gchar *signal_name);
