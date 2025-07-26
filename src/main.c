#include "gio/gio.h"
#include "glib-object.h"
#include "power_manager.h"
#include <polkit/polkit.h>
#include <glib.h>

static Powermanager *get_dbus_proxy(void)
{
    static Powermanager *proxy = NULL;
    static GDBusConnection *connection = NULL;
    GError *error = NULL;

    if (!proxy) {
        // Connect to D-Bus system bus once
        connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
        if (!connection) {
            g_printerr("Failed to connect to D-Bus: %s\n", error->message);
            g_error_free(error);
            return NULL;
        }

        // Create the proxy once
        proxy = powermanager_proxy_new_sync(connection,
                                            G_DBUS_PROXY_FLAGS_NONE,
                                            "com.mkps.powermanager",
                                            "/com/mkps/powermanager",
                                            NULL, &error);
        if (!proxy) {
            g_printerr("Failed to create proxy: %s\n", error->message);
            g_error_free(error);
        }
    }
    return proxy;
}

static void activate (GtkApplication *app, gpointer user_data)
{
  GObject  *button;
  t_app_data *data = (t_app_data *)user_data;
  /* Construct a GtkBuilder instance and load our UI description */

  data->proxy = get_dbus_proxy();
  if (!data->proxy) return;
  if (access_acpi(data->proxy, ACPI_CHK_PWR)) {
    fprintf(stderr, "Critical Error: could not get acpi data from dbus\n");
    return;
  }

  GtkBuilder *builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "/home/alx/Code/Powermanager/src/builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  GObject *window = gtk_builder_get_object(builder, "window");
  GObject *label = gtk_builder_get_object(builder, "current_mode");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_window_set_application (GTK_WINDOW (window), app);
  data->label = label;
  button = gtk_builder_get_object(builder, "btn_intelligent_cooling");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_IC), data);
  button = gtk_builder_get_object(builder, "btn_extreme_performance");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_EP), data);
  button = gtk_builder_get_object(builder, "btn_battery_saving");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_BS), data);
  update_PwrMode_text(data);

  GObject *buttonBC = gtk_builder_get_object(builder, "toggle_battery_conservation");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonBC), access_acpi(data->proxy, ACPI_CHK_BC));
  GObject *buttonRC = gtk_builder_get_object(builder, "toggle_rapid_charge");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonRC), access_acpi(data->proxy, ACPI_CHK_RC));
  data->buttonRC = buttonRC;
  g_signal_connect(buttonBC, "clicked", G_CALLBACK (switch_battery_bc), data);
  data->buttonBC = buttonBC;
  g_signal_connect(buttonRC, "clicked", G_CALLBACK (switch_battery_rc), data);

  gtk_widget_set_visible(GTK_WIDGET (window), TRUE);

  g_object_unref (builder);
}

int main (int   argc, char *argv[])
{
  int status;
  GtkApplication *app;

#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  t_app_data app_data = {0};
  app = gtk_application_new ("com.mkps.powermanager.app", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), &app_data);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return status;
}
