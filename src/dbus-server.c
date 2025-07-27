#include <gio/gio.h>
#include <stdlib.h>

#ifdef G_OS_UNIX
#include <gio/gunixfdlist.h>
/* For STDOUT_FILENO */
#include <unistd.h>
#include "power_manager.h"
#endif

#define DBUS_INTERFACE_PROPERTIES "org.freedesktop.DBus.Properties"

/* ---------------------------------------------------------------------------------------------------- */

gint execute_action(gint opcode) {
  int status = 0;
  printf("We got %i\n", opcode);
  switch (opcode) {
    case ACPI_CHK_BC:
      status = is_bc_on();
      break;
    case ACPI_CHK_RC:
      status = is_rc_on();
      break;
    case ACPI_SET_BC_OFF:
      write_acpi(ACPI_SET_BMODE_BC_OFF);
      break;
    case ACPI_SET_BC_ON:
      write_acpi(ACPI_SET_BMODE_BC_ON);
      break;
    case ACPI_SET_RC_OFF:
      write_acpi(ACPI_SET_BMODE_RC_OFF);
      break;
    case ACPI_SET_RC_ON:
      write_acpi(ACPI_SET_BMODE_RC_ON);
      break;
    case ACPI_CHK_PWR:
      status = acpi_query_pwrstatus();
      break;
    case ACPI_SET_IC:
      status = acpi_setpwr(ACPI_SET_PWRMODE_IC);
      break;
    case ACPI_SET_EP:
      status = acpi_setpwr(ACPI_SET_PWRMODE_EP);
      break;
    case ACPI_SET_BS:
      status = acpi_setpwr(ACPI_SET_PWRMODE_BS);
      break;
    default:
      status = -1;
      break;
  }
  return (gint)status;
}

static GDBusNodeInfo *introspection_data = NULL;

/* Introspection data for the service we are exporting */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='com.mkps.powermanager'>"
  "    <annotation name='com.mkps.powermanager.Annotation' value='OnInterface'/>"
  "    <method name='ExecuteCommand'>"
  "      <annotation name='com.mkps.powermanager.Annotation' value='OnMethod'/>"
  "      <arg type='i' name='opcode' direction='in'/>"
  "      <arg type='i' name='response' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";

/* ---------------------------------------------------------------------------------------------------- */

static void
handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  (void)user_data;
  (void)sender;
  (void)connection;
  (void)object_path;
  (void)interface_name;


  if (g_strcmp0 (method_name, "ExecuteCommand") == 0) {
    gint opcode;
    gint response;

    g_variant_get (parameters, "(i)", &opcode);
    response = execute_action(opcode);
    g_dbus_method_invocation_return_value (invocation, g_variant_new ("(i)", response));

  }
  else
  {
    g_dbus_method_invocation_return_dbus_error (invocation,
                                                "org.gtk.GDBus.Failed",
                                                "Your message bus daemon does not support file descriptor passing (need D-Bus >= 1.3.0)");
  }
}

static GVariant *
handle_get_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GError          **error,
                     gpointer          user_data)
{
  GVariant *ret;
  (void)connection;
  (void)object_path;
  (void)interface_name;
  (void)user_data;
  (void)sender;
  (void)property_name;
  (void)error;

  ret = NULL;
  return ret;
}

static gboolean
handle_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{
  (void)user_data;
  (void)connection;
  (void)sender;
  (void)object_path;
  (void)interface_name;
  (void)property_name;
  (void)value;
  return *error == NULL;
}


/* for now */
static const GDBusInterfaceVTable interface_vtable =
{
  handle_method_call,
  handle_get_property,
  handle_set_property,
  { 0 }
};

/* ---------------------------------------------------------------------------------------------------- */

/* static gboolean */
/* on_timeout_cb (gpointer user_data) */
/* { */
/*   GDBusConnection *connection = G_DBUS_CONNECTION (user_data); */
/*   GVariantBuilder *builder; */
/*   GVariantBuilder *invalidated_builder; */
/*   GError *error; */

/*   swap_a_and_b = !swap_a_and_b; */

/*   error = NULL; */
/*   builder = g_variant_builder_new (G_VARIANT_TYPE_ARRAY); */
/*   invalidated_builder = g_variant_builder_new (G_VARIANT_TYPE ("as")); */
/*   g_variant_builder_add (builder, */
/*                          "{sv}", */
/*                          "Foo", */
/*                          g_variant_new_string (swap_a_and_b ? "Tock" : "Tick")); */
/*   g_variant_builder_add (builder, */
/*                          "{sv}", */
/*                          "Bar", */
/*                          g_variant_new_string (swap_a_and_b ? "Tick" : "Tock")); */
/*   g_dbus_connection_emit_signal (connection, */
/*                                  NULL, */
/*                                  "/com/mkps/powermanager/TestObject", */
/*                                  DBUS_INTERFACE_PROPERTIES, */
/*                                  "PropertiesChanged", */
/*                                  g_variant_new ("(sa{sv}as)", */
/*                                                 "org.gtk.GDBus.TestInterface", */
/*                                                 builder, */
/*                                                 invalidated_builder), */
/*                                  &error); */
/*   g_assert_no_error (error); */


/*   return G_SOURCE_CONTINUE; */
/* } */

/* ---------------------------------------------------------------------------------------------------- */

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  (void)name;
  (void)user_data;
  guint registration_id;

  registration_id = g_dbus_connection_register_object (connection,
                                                       "/com/mkps/powermanager",
                                                       introspection_data->interfaces[0],
                                                       &interface_vtable,
                                                       NULL,  /* user_data */
                                                       NULL,  /* user_data_free_func */
                                                       NULL); /* GError** */
  g_assert (registration_id > 0);

  /* swap value of properties Foo and Bar every two seconds */
  /* g_timeout_add_seconds (2, */
  /*                        on_timeout_cb, */
  /*                        connection); */
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  (void)connection;
  (void)name;
  (void)user_data;
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  (void)connection;
  (void)name;
  (void)user_data;
  exit (1);
}

int
main (int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  guint owner_id;
  GMainLoop *loop;

  introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, NULL);
  if (geteuid() != 0) {
    fprintf(stderr, "Error: EUID is not root\n");
    exit(EXIT_FAILURE);
  }
  printf("1\n");

  g_assert (introspection_data != NULL);
  printf("2\n");

  owner_id = g_bus_own_name (G_BUS_TYPE_SYSTEM,
                             "com.mkps.powermanager",
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             on_name_acquired,
                             on_name_lost,
                             NULL,
                             NULL);
  printf("3\n");
  loop = g_main_loop_new (NULL, FALSE);
  printf("4\n");
  g_main_loop_run(loop);
  printf("5\n");

  g_bus_unown_name (owner_id);
  printf("6\n");

  g_dbus_node_info_unref (introspection_data);
  printf("7\n");

  return 0;
}
