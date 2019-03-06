#include "DBusService.h"

#include "DBusControls.h"
#include "glib-helpers/glib-object-helpers.h"

struct _DBusService
{
  GObject parent;
  DBusControls *interface;
  gchar *object_path;
  GDBusConnection *connection;
};

G_DEFINE_TYPE(DBusService, dbus_service, G_TYPE_OBJECT)

enum { PROPERTY_OBJECT_PATH = 1, N_PROPERTIES };

static void dbus_service_init(DBusService *self)
{
  self->interface = dbus_controls_skeleton_new();
}

static void dbus_service_set_property(DBusService *self,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *spec)
{
  switch (property_id)
  {
    case PROPERTY_OBJECT_PATH:
      g_assert(self->object_path == NULL);

      self->object_path = g_value_dup_string(value);

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, spec);
  }
}

static void dbus_service_dispose(DBusService *self)
{
  g_dbus_interface_skeleton_unexport_from_connection(G_DBUS_INTERFACE_SKELETON(
                                                       self->interface),
                                                     self->connection);

  g_clear_object(&self->interface);
  g_clear_object(&self->connection);

  G_OBJECT_CLASS(dbus_service_parent_class)->dispose(G_OBJECT(self));
}

static void dbus_service_finalize(DBusService *self)
{
  g_clear_pointer(&self->object_path, g_free);

  G_OBJECT_CLASS(dbus_service_parent_class)->finalize(G_OBJECT(self));
}

static void dbus_service_class_init(DBusServiceClass *klass)
{
  GObjectClass *object_class           = G_OBJECT_CLASS(klass);
  GParamSpec *properties[N_PROPERTIES] = {
    NULL,
  };

  object_class->dispose  = (GObjectDisposeFunc) dbus_service_dispose;
  object_class->finalize = (GObjectFinalizeFunc) dbus_service_finalize;
  object_class->set_property =
    (GObjectSetPropertyFunc) dbus_service_set_property;

  properties[PROPERTY_OBJECT_PATH] =
    g_param_spec_string("object-path",
                        "Object path",
                        "D-Bus object path",
                        NULL,
                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

  g_object_class_install_properties(object_class, N_PROPERTIES, properties);
}

DBusService *dbus_service_new(const gchar *object_path)
{
  g_return_val_if_fail(object_path != NULL, NULL);

  return g_object_new(DBUS_TYPE_SERVICE, "object-path", object_path, NULL);
}

void dbus_service_register(DBusService *self, GDBusConnection *connection)
{
  g_return_if_fail(DBUS_IS_SERVICE(self));
  g_return_if_fail(G_IS_DBUS_CONNECTION(connection));

  if (self->object_path == NULL)
  {
    g_error("Object path must be set before exporting an interface.");
  }

  g_set_object(&self->connection, connection);

  g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(self->interface),
                                   self->connection,
                                   self->object_path,
                                   NULL);
}

void dbus_service_emit_signal(DBusService *self, const gchar *signal_name)
{
  g_return_if_fail(DBUS_IS_SERVICE(self));
  g_return_if_fail(signal_name != NULL);

  g_signal_emit_by_name(self->interface, signal_name);
}
